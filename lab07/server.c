#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <string.h>
#include <fcntl.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <errno.h>

#include "common.h"

int main() {

  mqd_t clients[CLIENT_LIMIT];


  struct mq_attr attr = {
    .mq_flags = 0,
    .mq_maxmsg = 10,
    .mq_msgsize = sizeof(message_t),
    .mq_curmsgs = 0
  };

  mqd_t server_mq = mq_open(SERVER_QUEUE_NAME, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR, &attr);

  if(server_mq == -1) {
    printf("Creating mqueue failed!\n");
    return EXIT_FAILURE;
  }

  int id = 0;
  int must_stop = 0;
  while (!must_stop) 
  {
    message_t received_msg;

    if(mq_receive(server_mq, (char*)&received_msg, sizeof(message_t), NULL) == -1){
      printf("Reading has failed!\n");
      printf("%s\n", strerror(errno));
      must_stop = 1;
    }

    if(!strcmp(received_msg.type, MSG_INIT)) {
      printf("Received INIT ask\n");
      if(id >= CLIENT_LIMIT) {
        printf("Client limit has been exceeded, skipping\n");
        continue;
      }

      mqd_t client_mq = mq_open(received_msg.text, O_RDWR, S_IRUSR | S_IWUSR, NULL);

      if(client_mq == -1) {
        printf("Opening mqueue for client failed, skipping this client\n");
        continue;
      }
      
      clients[id] = client_mq;
      
      message_t sent_message = {
        .type = MSG_INIT,
        .identifier = id
      };

      if(mq_send(clients[id], (char*)&sent_message, sizeof(sent_message), 1) == -1) {
        printf("Couldn't send response to client... interesting\n");
      }

      printf("INIT client number %d, with identifier = %d\n", id, id);
      id++;
    }
    
    else if(!strcmp(received_msg.type, MSG_TEXT)) {
      printf("Received msg identifier: %d\n", received_msg.identifier);
      for(int i=0; i<id; i++) {
        if(received_msg.identifier != i) {
          printf("%d ||| %d\n", received_msg.identifier, clients[i]);
          if(mq_send(clients[i], (char*)&received_msg, sizeof(received_msg), 1) == -1) {
            printf("Couldn't send message to client... interesting\n");
          }
        }
      }
      printf("Sent following message: %s\n", received_msg.text);
    }

    else if(!strcmp(received_msg.type, MSG_KILL)) {
      must_stop = 1;
      for(int i=0; i<id; i++) {
        if(mq_send(clients[i], (char*)&received_msg, sizeof(received_msg), 1) == -1) {
          printf("Couldn't send KILLING message to client... interesting\n");
        }
      }
      printf("KILL has been received, adios!");
    }

  }
  

  if(mq_close(server_mq)) {
    printf("Closing mqueue unsuccessful :(\n");
  }

  if(mq_unlink(SERVER_QUEUE_NAME)) {
    printf("Deleting mqueue unsuccessful :(\n");
    printf("%s\n", strerror(errno));
  }
}