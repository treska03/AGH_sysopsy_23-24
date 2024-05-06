#ifndef COMMON_H_
#define COMMON_H_

#define SERVER_QUEUE_NAME  "/server_queue"
#define MAX_SIZE    1024
#define MSG_SIZE    2048
#define CLIENT_LIMIT 10
#define MSG_INIT    "INIT"
#define MSG_TEXT    "TEXT"
#define MSG_KILL    "KILL"

typedef struct {
    char type[5];
    char text[MAX_SIZE];
    mqd_t identifier;
} message_t;

#endif