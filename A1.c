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
    char filepath[100];

    while (1)
    {
        init_queue();

        system("clear");
        printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
        printf("@@           Link A1            @@\n");
        printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");

        printf("\nChoose a file to be sent: \n\n");
        scanf("%s", filepath);
        printf("\n");

        FILE *file = fopen(filepath, "r");

        char tmp;
        int count = 0;

        struct stat sb;
        stat(filepath, &sb);

        int packages_count = 0;
        int total_packages = sb.st_size / MAX + (sb.st_size % MAX != 0 ? 1 : 0);

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
                printf("Package [%d/%d]: [%s] \n", ++packages_count, total_packages, message.mesg_text);
            }
        }

        if (count != 0)
        {
            message.mesg_text[count] = '\0';

            // msgsnd to send message
            msgsnd(msgid, &message, sizeof(message), 0);
            printf("Package [%d/%d]: [%s] \n", ++packages_count, total_packages, message.mesg_text);
        }

        printf("\nEverything sent! ;)\n\n");

        printf("Press enter to send another or Ctrl+C to quit...");
        getchar();
        getchar();
    }

    return 0;
}