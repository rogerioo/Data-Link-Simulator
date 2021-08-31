// C Program for Message Queue (Writer Process)
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <mqueue.h>

#define MAX 10
#define QUEUE_ID 65

// structure for message queue
struct mesg_buffer
{
    long mesg_type;
    char mesg_text[100];
} message;

int msgid;

void init_queue()
{
    msgid = msgget(QUEUE_ID, 0666 | IPC_CREAT | IPC_NOWAIT);
    message.mesg_type = 1;
}

int main()
{
    init_queue();

    char filepath[100];

    printf("Escolha o arquivo a ser enviado : ");
    scanf("%s", filepath);

    FILE *file = fopen(filepath, "r");

    char tmp;
    int count = 0;

    while ((tmp = fgetc(file)) != EOF)
    {
        message.mesg_text[count++] = tmp;

        if (count == MAX)
        {
            if (tmp == EOF)
                message.mesg_text[MAX] = '\0';

            count = 0;

            // msgsnd to send message
            msgsnd(msgid, &message, sizeof(message), 0);
            // display the message
            printf("Data send is : %s \n", message.mesg_text);
        }
    }

    if (count != 0)
    {
        message.mesg_text[count] = '\0';

        // msgsnd to send message
        msgsnd(msgid, &message, sizeof(message), 0);
        // display the message
        printf("Data send is : %s \n", message.mesg_text);
    }

    return 0;
}