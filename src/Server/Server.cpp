#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "Shared.hpp"
#include "Message.hpp"
#include "PixelBroadcastMessage.hpp"
#include "SerialData.hpp"

#define PRINT_DEBUG 1

#define ERR()                                                                                    \
    do                                                                                           \
    {                                                                                            \
        fprintf(stderr, "[ ERROR ]: %s [ AT ]: %s():%d\n", strerror(errno), __func__, __LINE__); \
        exit(-1);                                                                                \
    } while (0)

#define DBG(...)                                              \
    do                                                        \
    {                                                         \
        if (PRINT_DEBUG)                                      \
        {                                                     \
            printf("[ DEBUG ]: ");                            \
            printf(__VA_ARGS__);                              \
            printf(" [ AT ]: %s():%d\n", __func__, __LINE__); \
        }                                                     \
    } while (0)

int openServerFD(in_addr_t ip, in_port_t port);

int runPollLoop(int serverFD, uint maxClients);

void handleServerPollin(int serverFD, int maxClients, struct pollfd *pPollFDs, int *pNPollFDs);

void readByteToStream(int fdIndex, SerialData *inDataStreams, struct pollfd *pPollFDs);

void handleClientMessage(int fdIndex, struct pollfd *pPollFDs, int nPollFDs, int serverFD, SerialData *inDataStreams);

void handleClientPollin(int fdIndex, struct pollfd *pPollFDs, int nPollFDs, int serverFD, SerialData *inDataStreams);

void handlePixelBroadcast(int serverFD, int fdIndex, struct pollfd *pPollFDs, int nPollFDs, SerialData recvMessageData);

void handleHangup(int fdIndex, struct pollfd *pPollFDs);

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        DBG("Not enough arguments. Arguments should be: serverIP");
        return 1;
    }

    const char *serverIP = argv[1];

    const int serverFD = openServerFD(inet_addr(serverIP), htons(5341));
    const uint maxClients = 10;

    DBG("Starting polling");
    runPollLoop(serverFD, maxClients);

    close(serverFD);
    return 0;
}

void handleHangup(int fdIndex, struct pollfd *pPollFDs)
{
    close(pPollFDs[fdIndex].fd);
    pPollFDs[fdIndex].events = -1;
    pPollFDs[fdIndex].fd = -1;
}

void handleServerPollin(int serverFD, int maxClients, struct pollfd *pPollFDs, int *pNPollFDs)
{
    if (*pNPollFDs == maxClients)
    {
        DBG("Cannot accept new client, already at max.");
        return;
    }
    const int newClientFD = accept(serverFD, NULL, NULL);
    if (newClientFD == -1)
    {
        ERR();
    }
    pPollFDs[*pNPollFDs].fd = newClientFD;
    pPollFDs[*pNPollFDs].events = POLLIN;
    DBG("Accepted client FD %d", newClientFD);
    (*pNPollFDs)++;
}

void handlePixelBroadcast(int serverFD, int fdIndex, struct pollfd *pPollFDs, int nPollFDs, SerialData recvMessageData)
{
    const PixelBroadcastMessage recvPixel = [&]()
    {
        PixelBroadcastMessage recvPixel;
        recvPixel.init(recvMessageData);
        return recvPixel;
    }();

    DBG("recvMessageData: %s", recvMessageData.toString().c_str());

    const PixelBroadcastMessage sendPixel = recvPixel;
    const SerialData sendPixelData = sendPixel.serialize();

    DBG("sendPixelData: %s", sendPixelData.toString().c_str());

    printf("\n");

    for (nfds_t i = 0; i < nPollFDs; i++)
    {
        if (pPollFDs[i].fd == -1)
        {
            continue;
        }

        if (pPollFDs[i].fd == serverFD)
        {
            continue;
        }

        size_t bytesRemaining = sendPixelData.size();

        while (bytesRemaining > 0)
        {
            const void *bufBegin = &sendPixelData.data()[sendPixelData.size() - bytesRemaining];
            const size_t bytesWritten = write(pPollFDs[i].fd, bufBegin, bytesRemaining);
            if (bytesWritten == -1)
            {
                DBG("Failed to write to FD %d. Reason: %s", i, strerror(errno));
                break;
            }
            bytesRemaining -= bytesWritten;
        }
    }
}

void readByteToStream(int fdIndex, SerialData *inDataStreams, struct pollfd *pPollFDs)
{
    size_t bytesRemaining = 1;
    uint8_t byteIn = 0;

    while (bytesRemaining > 0)
    {
        const size_t bytesRead = read(pPollFDs[fdIndex].fd, &byteIn, 1);
        bytesRemaining -= bytesRead;
    }

    inDataStreams[fdIndex].push_back(byteIn);
}

void handleClientMessage(int fdIndex, struct pollfd *pPollFDs, int nPollFDs, int serverFD, SerialData *inDataStreams)
{
    const Message::Info recvMessageInfo = Message::deserialize(inDataStreams[fdIndex]);

    switch (recvMessageInfo.type)
    {
    case Message::PIXEL_BROADCAST_MSG:
        handlePixelBroadcast(serverFD, fdIndex, pPollFDs, nPollFDs, recvMessageInfo.data);
    default:
        break;
    }

    inDataStreams[fdIndex].clear();
}

void handleClientPollin(int fdIndex, struct pollfd *pPollFDs, int nPollFDs, int serverFD, SerialData *inDataStreams)
{
    readByteToStream(fdIndex, inDataStreams, pPollFDs);

    const uint8_t expectedMessageSize = inDataStreams[fdIndex][0];

    if (inDataStreams[fdIndex].size() == expectedMessageSize)
    {
        handleClientMessage(fdIndex, pPollFDs, nPollFDs, serverFD, inDataStreams);
    }
}

int openServerFD(in_addr_t ip, in_port_t port)
{
    const int serverFD = socket(AF_INET, SOCK_STREAM, 0);
    const struct sockaddr_in serverSockAddr = {.sin_family = AF_INET, .sin_addr.s_addr = ip, .sin_port = port};

    if (serverFD == -1)
    {
        ERR();
    }
    if (bind(serverFD, (struct sockaddr *)&serverSockAddr, sizeof(serverSockAddr)) == -1)
    {
        ERR();
    }
    if (listen(serverFD, 2) == -1)
    {
        ERR();
    }

    return serverFD;
}

int runPollLoop(int serverFD, uint maxClients)
{
    const struct pollfd howToPollServer = {.fd = serverFD, .events = POLLIN};
    struct pollfd pollFDs[maxClients + 1];
    SerialData inDataStreams[maxClients + 1];

    int nPollFDs = 1;
    pollFDs[0] = howToPollServer;

    while (nPollFDs > 0)
    {
        const int pollReturned = poll(pollFDs, nPollFDs, -1);

        if (pollReturned < 0)
        {
            ERR();
        }
        else
        {
            for (nfds_t i = 0; i < nPollFDs; i++)
            {
                const int retEvent = pollFDs[i].revents;

                if (retEvent & (POLLERR | POLLHUP))
                {
                    handleHangup(i, pollFDs);
                }
                else if (retEvent & POLLIN)
                {
                    if (pollFDs[i].fd == serverFD)
                    {
                        handleServerPollin(serverFD, maxClients, pollFDs, &nPollFDs);
                    }
                    else
                    {
                        handleClientPollin(i, pollFDs, nPollFDs, serverFD, inDataStreams);
                    }
                }
            }
        }
    }

    return 0;
}
