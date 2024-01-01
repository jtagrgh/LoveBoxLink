/*
    This should run on a computer who's IP is findable by the two LoveBoxes.
    LoveBoxes will send buffers of changes of some fixed size to the server.
    The server propogates the changes to the other LoveBox.
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void *handle_client(void *arg);

int main() {
    int                 server_fd;
    struct sockaddr_in  server_addr;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Failed to create socket");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = inet_addr("192.168.2.134");

    
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Failed to bind");
        return 1;
    }

    if (listen(server_fd, 10) == -1) {
        perror("Listen failed");
    }

    while (1) {
        int                 *client_fd      = malloc(sizeof(int));
        struct sockaddr_in  client_addr;
        socklen_t           client_addr_len = sizeof(client_addr);

        *client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
        if (*client_fd == -1) {
            perror("Accept failed");
            continue;
        }

        pthread_t thread_id;
        pthread_create(&thread_id, NULL, handle_client, (void *)client_fd);
        pthread_detach(thread_id);
    }

    return 0;
}

void *handle_client(void *arg) {
    int client_fd = *((int *) arg);
    printf("Handling client %d\n", client_fd);
    close(client_fd);
    free(arg);
    return NULL;
}
