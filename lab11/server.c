#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

volatile int g_keep_alive = 1;

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024
#define MAX_IDENTIFIER_LEN 63

typedef struct {
    int socket;
    char id[MAX_IDENTIFIER_LEN];
    int id_len;
} client_t;

client_t clients[MAX_CLIENTS];

pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void* handler(int client_socket) {
    char id[MAX_IDENTIFIER_LEN];
    char buff[BUFFER_SIZE];

    int id_len = recv(client_socket, id, MAX_IDENTIFIER_LEN, 0); // CHECK IF NOT &id
    

    pthread_mutex_lock(&clients_mutex);

    for(int i=0; i<MAX_IDENTIFIER_LEN; i++) {
        if(clients[i].socket == 0) {
            clients[i].socket = client_socket;
            strcpy(clients[i].id, id);
            clients[i].id_len = id_len;
            break;
        }
    }

    pthread_mutex_unlock(&clients_mutex);

    while(g_keep_alive) {
        int received = recv(client_socket, buff, BUFFER_SIZE, 0); // &buff?
        if(received == -1) perror("recv");

        if (strncmp(buff, "LIST", 4) == 0) {
            for(int i=0; i<MAX_CLIENTS; i++) {
                if(clients[i].socket != 0) {
                    send(client_socket, clients[i].id, clients[i].id_len, 0);
                    send(client_socket, "\n", 1, 0);
                }
            }
        } else if (strncmp(buff, "2ALL", 4) == 0) {
            char message[BUFFER_SIZE + 50];
            time_t now = time(0);
            strftime(message, 50, "[%Y-%m-%d %H:%M:%S] ", localtime(&now));
            strcat(message, id);
            strcat(message, ": ");
            strcat(message, buff + 5);

            pthread_mutex_lock(&clients_mutex);
            for(int i=0; i<MAX_CLIENTS; i++) {
                if(clients[i].socket != 0 && clients[i].socket != client_socket) {
                    send(clients[i].socket, &message, strlen(message), 0);
                }
            }
            pthread_mutex_unlock(&clients_mutex);
        } else if (strncmp(buff, "2ONE", 4) == 0) {
            char* recipient_id = strtok(buff + 5, " ");
            char* message = strtok(NULL, "");
            char full_message[BUFFER_SIZE + 50];
            time_t now = time(0);
            strftime(full_message, 50, "[%Y-%m-%d %H:%M:%S] ", localtime(&now));
            strcat(full_message, id);
            strcat(full_message, ": ");
            strcat(full_message, message);
            pthread_mutex_lock(&clients_mutex);
            for (int i = 0; i < MAX_CLIENTS; ++i) {
                if (clients[i].socket != 0 && strcmp(clients[i].id, recipient_id) == 0) {
                    send(clients[i].socket, full_message, strlen(full_message), 0);
                    break;
                }
            }
            pthread_mutex_unlock(&clients_mutex);
        } else if (strncmp(buff, "STOP", 4) == 0) {
            break;
        }
    }

    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i].socket == client_socket) {
            clients[i].socket = 0;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
    
    close(client_socket);
    pthread_exit(0);
}

void* check_alive_clients(void*) {
    while (1) {
        sleep(3);
        pthread_mutex_lock(&clients_mutex);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].socket != 0) {
                if (send(clients[i].socket, "ALIVE", 5, 0) <= 0) {
                    close(clients[i].socket);
                    clients[i].socket = 0;
                }
            }
        }
        pthread_mutex_unlock(&clients_mutex);
    }
    return NULL;
}


int main(int argc, char* argv[]) {
    if(argc != 2) {
        printf("Please provide port\n");
        return EXIT_FAILURE;
    }

    // socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(server_socket == -1) perror("socket");

    // bind
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[1])); // potentially its not htons, since atoi could return 32-bit
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) perror("bind");

    // listen
    if(listen(server_socket, MAX_CLIENTS) == -1) perror("liten");


    pthread_t thread_id;
    int client_socket;

    pthread_create(&thread_id, NULL, check_alive_clients, NULL);
    
    while(g_keep_alive) {
        // accept
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);

        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
        printf("Accepted someone\n");
        if(client_socket == -1) perror("accept");

        pthread_create(&thread_id, NULL, (void*)handler, (void *)(intptr_t)client_socket);
        
    }

    if(shutdown(server_socket, SHUT_RDWR) == -1) perror("shutdown");
    if(close(server_socket) == -1) perror("close");

    printf("Hello\n");

    return EXIT_SUCCESS;
}