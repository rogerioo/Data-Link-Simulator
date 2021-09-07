// C Program for Message Queue (Writer Process)
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <mqueue.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <sys/stat.h>

#define MAX 10
struct mesg_buffer
{
    long mesg_type;
    char mesg_text[100];
};

int init_queue(int queue_id)
{
    return msgget(queue_id, 0666 | IPC_CREAT | IPC_NOWAIT);
}
void *sendFile(void *vargp)
{
    struct mesg_buffer message;
    int queue_id = *((int *)vargp);

    while (1)
    {
        
        int msgid = init_queue(queue_id);
        message.mesg_type = 1;

        char filepath[100];
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
}

void *readMessages(void *vargp)
{
    char complete_message[100000];
    int queue_id = *((int *)vargp);
    struct mesg_buffer message;
    struct msqid_ds buf;
    while (1)
    {
        int hasRead = 0;
        int msgid = init_queue(queue_id);

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
                printf("Received file: [%s]\n", complete_message);
                break;
            }

            msgrcv(msgid, &message, sizeof(message), 1, 0);
            char temp[1000];
            strcpy(temp, message.mesg_text);
            strcat(complete_message, temp);
            hasRead = 1;
            sleep(0.5);
        }
        sleep(5);
    }
}
int main(int argc, char* argv[])
{
    pthread_t thread_sendFile_id, thread_readMessages_id;
    int send_id = atoi(argv[1]);
    int receive_id = atoi(argv[2]);
    pthread_create(&thread_readMessages_id, NULL, readMessages, (void *) &receive_id);
    pthread_create(&thread_sendFile_id, NULL, sendFile, (void *) &send_id);
    pthread_join(thread_readMessages_id, NULL);
    pthread_join(thread_sendFile_id, NULL);
    return 0;
}