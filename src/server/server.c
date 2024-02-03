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

#define MAX_CLIENTS 10


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

int runPollLoop(int serverFD) {
  const struct pollfd serverPollSpec = {
    .fd = serverFD, 
    .events = POLLIN
  };
  struct pollfd pollFDs[MAX_CLIENTS + 1];
  int nPollFDs = 1;
  struct Message recvMessageBuf;
  pollFDs[0] = serverPollSpec;

  while (nPollFDs > 0) {
    const int pollReturned = poll(pollFDs, nPollFDs, -1);

    if (pollReturned == 0) {
      /* Poll timed out, i.e., no  subscribed events noticed */
    }
    else if (pollReturned < 0) {
      ERR();
    } 
    else {
      for (nfds_t i = 0; i < nPollFDs; i++) {
        const int rEvent = pollFDs[i].revents;
        if (rEvent == 0) { /* No event on this FD */
          /* Continue to next FD */
        }
        else if (rEvent & (POLLERR | POLLHUP)) { /* Client errored or hung up */
          /* Remove client from polling list */
          close(pollFDs[i].fd);
          pollFDs[i].events = -1;
          pollFDs[i].fd = -1;
        }
        else if (rEvent & POLLIN) { /* Meaninful event recieved from client */
          if (pollFDs[i].fd == serverFD) { /* Event comes from server FD */
            /* Accept new client if possible */
            int clientFD = accept(serverFD, NULL, NULL);
            if (clientFD == -1) { 
              ERR(); 
            }
            pollFDs[nPollFDs].fd = clientFD;
            pollFDs[nPollFDs].events = POLLIN;
            DBG("Accepted client FD %d", clientFD);
            nPollFDs++;
          }
          else { /* Event comes from a client */
            recv(pollFDs[i].fd, &recvMessageBuf, sizeof(struct Message), MSG_WAITALL);
            printf("Read from FD %d: x %d y %d colour %d\n", i, recvMessageBuf.x, recvMessageBuf.y, recvMessageBuf.colour);
            for (nfds_t j = 0; j < nPollFDs; j++) {
              if (pollFDs[j].fd == -1) {
                continue;
              }
              recvMessageBuf.colour = 0x03E0;
              recvMessageBuf.x += 10;
              recvMessageBuf.y += 10;
              write(pollFDs[j].fd, (const void*)&recvMessageBuf, messageSize);
            }
          }
        }
      }
    }
  }

  return 0;
}

int main() {
  const int serverFD = openServerFD(inet_addr("192.168.1.198"), htons(5341));
  DBG("Server started");
  runPollLoop(serverFD);

  close(serverFD);
  return 0;
}

