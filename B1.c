// C Program for Message Queue (Reader Process)
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#define QUEUE_ID 62

// structure for message queue
struct mesg_buffer
{
    long mesg_type;
    char mesg_text[100];
} message;

int msgid;
struct msqid_ds buf;

char complete_message[100000];

void init_queue()
{
    msgid = msgget(QUEUE_ID, 0666 | IPC_CREAT | IPC_NOWAIT);
}

int main()
{
    while (1)
    {
        system("clear");
        printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
        printf("@@           Link B1            @@\n");
        printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");

        printf("\nWaiting for messages in the queue...\n\n");

        int hasRead = 0;
        init_queue();

        memset(complete_message, 0, sizeof(complete_message));

        while (1)
        {
            int n = msgctl(msgid, IPC_STAT, &buf);
            if (n < 0)
            {
                perror("Error on reading queue");
                exit(1);
            }
            if (buf.msg_qnum <= 0 && hasRead)
            {
                printf("Complete message received: [%s]\n", complete_message);
                break;
            }

            msgrcv(msgid, &message, sizeof(message), 1, 0);
            strcat(complete_message, message.mesg_text);
            hasRead = 1;
            sleep(0.5);
        }

        sleep(5);
    }

    return 0;
}