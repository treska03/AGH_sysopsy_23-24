#include <stdio.h>
#include <mqueue.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <errno.h>
#include <signal.h>


#include "common.h"

int must_stop = 0;

void intHandler(int signo) {
  must_stop = 1;
}

int main() {
  signal(SIGINT, intHandler);

  pid_t curr_pid = getpid();
  char queue_name[63] = {0};
  sprintf(queue_name, "%s_client_%d", SERVER_QUEUE_NAME, curr_pid);
  printf("%s\n", queue_name);

  struct mq_attr attr = {
    .mq_flags = 0,
    .mq_msgsize = sizeof(message_t),
    .mq_maxmsg = 10
  };
  // client_mq : server -> client
  mqd_t client_mq = mq_open(queue_name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, &attr);
  if(client_mq == -1) {
    must_stop = 1;
    printf("Opening server -> client mqueue has failed\n");
    printf("%s\n", strerror(errno));
  }

  // server_mq : client -> server
  mqd_t server_mq = mq_open(SERVER_QUEUE_NAME, O_RDWR, S_IRUSR | S_IWUSR, NULL);
  if(server_mq == -1) {
    must_stop = 1;
    printf("Opening client -> server mqueue has failed\n");
  } 

  mqd_t id;
  
  message_t msg = {
    .type = MSG_INIT,
    .identifier = -1
  };

  strcpy(msg.text, queue_name);

  if(mq_send(server_mq, (char*)&msg, sizeof(msg), 1) == -1) {
    printf("INIT didn't work\n");
    printf("%s\n", strerror(errno));
  }

  printf("Waiting for INIT response...\n");

  message_t response;
  mq_receive(client_mq, (char*)&response, sizeof(response), NULL);
  
  printf("INIT client with identifier = %d\n", response.identifier);
  id = response.identifier;

  pid_t pid = fork();
  if(pid == -1) {
    must_stop = 1;
    printf("Oh no, fork failed\n");
  }

  if(pid == 0 && !must_stop) {
    // we're in child process

    while(!must_stop) {
      message_t received;

      if(mq_receive(client_mq, (char*)&received, sizeof(received), NULL) == -1) {
        printf("Failed to receive message smh\n");
      };
      
      if(!strcmp(received.type, MSG_TEXT)) {
        printf("Received message: %s\n", received.text);
      }
      else {
        printf("Server sent kill message, exiting\n");
        exit(0);
      }
    }
  }

  else if(pid > 0 && !must_stop){
    // we're in parent process

    char* buffer = NULL;
    while(!must_stop) {
      if(scanf("%ms", &buffer) == 1) {
        // setup message for sending to server
        if(must_stop) break;
        
        message_t send_message = {
          .type = MSG_TEXT,
          .identifier = id
        };

        if(!strncmp(buffer, MSG_KILL, sizeof(MSG_KILL))) {
          printf("Kill order has been put on server! Executing\n");
          memmove(send_message.type, MSG_KILL, sizeof(MSG_KILL));
        }

        // copy message to message buffer, MIN is used to prevent buffer overflow
        memcpy(send_message.text, buffer, strlen(buffer));

        // send message to server
        mq_send(server_mq, (char*)&send_message, sizeof(send_message), 1);

        // free memory allocated by scanf
        free(buffer);
        buffer = NULL;
        printf("Message sent!\n");
      }
      else {
        printf("Scanning error\n");
        must_stop = 1;
      }
    }
  }

  printf("Waiting for message\n");
  message_t received;
  printf("%s |||| received\n", received.text);
  mq_close(server_mq);
  mq_close(client_mq);
  if(!mq_unlink(queue_name)) printf("didnt delete client queue\n");

}