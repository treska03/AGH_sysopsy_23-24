#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <pthread.h>

#define BUFFER_SIZE 1024
#define MAX_IDENTIFIER_LEN 63

int server_socket;
char client_id[MAX_IDENTIFIER_LEN];

void* receive_messages(void*) {
    char buff[BUFFER_SIZE];
    while (1) {
        int received = recv(server_socket, buff, BUFFER_SIZE, 0);
        if (received <= 0) {
            break;
        }
        if(strncmp(buff, "ALIVE", 5) != 0) {
            buff[received] = '\0';
            printf("%s\n", buff);
        }
    }
    return NULL;
}

void cleanup() {
    send(server_socket, "STOP", 4, 0);
    close(server_socket);
    exit(0);
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <client_id> <server_ip> <server_port>\n", argv[0]);
        return 1;
    }

    strcpy(client_id, argv[1]);
    char* server_ip = argv[2];
    int server_port = atoi(argv[3]);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        return 1;
    }

    if (connect(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        return 1;
    }

    send(server_socket, client_id, strlen(client_id), 0);

    signal(SIGINT, cleanup);

    pthread_t thread_id;
    pthread_create(&thread_id, NULL, receive_messages, NULL);

    char buff[BUFFER_SIZE];
    while (1) {
        fgets(buff, BUFFER_SIZE, stdin);
        buff[strcspn(buff, "\n")] = '\0';

        if (strncmp(buff, "LIST", 4) == 0 || strncmp(buff, "STOP", 4) == 0) {
            send(server_socket, buff, strlen(buff), 0);
            if (strncmp(buff, "STOP", 4) == 0) {
                break;
            }
        } else if (strncmp(buff, "2ALL", 4) == 0 || strncmp(buff, "2ONE", 4) == 0) {
            send(server_socket, buff, strlen(buff), 0);
        } else {
            printf("Unknown command\n");
        }
    }

    close(server_socket);
    return 0;
}
