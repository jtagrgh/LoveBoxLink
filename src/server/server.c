#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "../shared/shared.h"



#define PRINT_DEBUG 1

#define ERR() do { \
  fprintf(stderr, "[ ERROR ]: %s [ AT ]: %s():%d\n", strerror(errno), __func__, __LINE__); \
  exit(-1); \
} while(0)

#define DBG(...) do { \
  if (PRINT_DEBUG) { \
    printf("[ DEBUG ]: "); \
    printf(__VA_ARGS__); \
    printf(" [ AT ]: %s():%d\n", __func__, __LINE__); \
  } \
} while(0)



int openServerFD(in_addr_t ip, in_port_t port);

int runPollLoop(int serverFD, uint maxClients);

void handleServerPollin(int serverFD, int maxClients, struct pollfd* pPollFDs, int* pNPollFDs);

void handleClientPollin(int clientFD, struct pollfd* pPollFDs, int nPollFDs);

void handleHangup(int fdIndex, struct pollfd* pPollFDs);



int main() {
  const int serverFD = openServerFD(inet_addr("192.168.1.198"), htons(5341));
  const uint maxClients = 10;

  runPollLoop(serverFD, maxClients);

  close(serverFD);
  return 0;
}

void handleHangup(int fdIndex, struct pollfd* pPollFDs) {
  close(pPollFDs[fdIndex].fd);
  pPollFDs[fdIndex].events = -1;
  pPollFDs[fdIndex].fd = -1;
}

void handleServerPollin(int serverFD, int maxClients, struct pollfd* pPollFDs, int* pNPollFDs) {
  if (*pNPollFDs == maxClients) {
    DBG("Cannot accept new client, already at max.");
    return;
  }
  const int newClientFD = accept(serverFD, NULL, NULL);
  if (newClientFD == -1) { 
    ERR(); 
  }
  pPollFDs[*pNPollFDs].fd = newClientFD;
  pPollFDs[*pNPollFDs].events = POLLIN;
  DBG("Accepted client FD %d", newClientFD);
  (*pNPollFDs)++;
}

void handleClientPollin(int clientFD, struct pollfd* pPollFDs, int nPollFDs) {
  struct Message recvMessageBuf;

  recv(pPollFDs[clientFD].fd, &recvMessageBuf, messageSize, MSG_WAITALL);
  DBG("Read from FD %d: x %d y %d colour %d\n", clientFD, recvMessageBuf.x, recvMessageBuf.y, recvMessageBuf.colour);

  for (nfds_t j = 0; j < nPollFDs; j++) {
    if (pPollFDs[j].fd == -1) {
      continue;
    }
    recvMessageBuf.colour = 0x03E0;
    recvMessageBuf.x += 10;
    recvMessageBuf.y += 10;
    write(pPollFDs[j].fd, (const void*)&recvMessageBuf, messageSize);
  }
}

int openServerFD(in_addr_t ip, in_port_t port) {
  const int serverFD = socket(AF_INET, SOCK_STREAM, 0);
  const struct sockaddr_in serverSockAddr = {.sin_family = AF_INET, .sin_addr.s_addr = ip, .sin_port = port};

  if (serverFD == -1) { 
    ERR(); 
  }
  if (bind(serverFD, (struct sockaddr *)&serverSockAddr, sizeof(serverSockAddr)) == -1) { 
    ERR(); 
  }
  if (listen(serverFD, 2) == -1) { 
    ERR(); 
  }

  return serverFD;
}

int runPollLoop(int serverFD, uint maxClients) {
  const struct pollfd howToPollServer = {.fd = serverFD, .events = POLLIN};
  struct pollfd pollFDs[maxClients + 1];
  int nPollFDs = 1;
  pollFDs[0] = howToPollServer;

  while (nPollFDs > 0) {
    const int pollReturned = poll(pollFDs, nPollFDs, -1);

    if (pollReturned < 0) {
      ERR();
    } 
    else {
      for (nfds_t i = 0; i < nPollFDs; i++) {
        const int retEvent = pollFDs[i].revents;
        
        if (retEvent & (POLLERR | POLLHUP)) {
          handleHangup(i, pollFDs);
        }
        else if (retEvent & POLLIN) {
          if (pollFDs[i].fd == serverFD) {
            handleServerPollin(serverFD, maxClients, pollFDs, &nPollFDs);
          }
          else {
            handleClientPollin(i, pollFDs, nPollFDs);
          }
        }
      }
    }
  }

  return 0;
}


