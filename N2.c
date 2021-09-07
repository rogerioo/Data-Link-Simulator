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
#include <pthread.h>
#include <string.h>
#include <sys/stat.h>

#define localhost "127.0.0.1"
#define PORT 8081
#define MAX 10

// structure for message queue
struct mesg_buffer
{
    long mesg_type;
    char mesg_text[100];
};

int sd, rc;
struct sockaddr_in endServ;
struct sockaddr_in endCli;
int should_bind;

void send_message(const char *msg)
{
    int n;
    if (should_bind) {
        n = sendto(sd, msg, strlen(msg), 0,
                   (const struct sockaddr *)&endCli, sizeof(endCli));
    } else {
        n = sendto(sd, msg, strlen(msg), 0,
                    (const struct sockaddr *)&endServ, sizeof(endServ));

    }
    if (n < 0)
    {
        printf("Falha ao enviar mensagem: [%s]\n", msg);
        //perror("Erro ao enviar mensagem");
        //exit(1);
    }
}

void init_udp(int port)
{
    /* Criacao do socket UDP */
    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sd < 0)
    {
        printf("Fail to create server\n");
        exit(1);
    }
    else
    {
        printf("Server was created successfully\n");
    }

    endServ.sin_family = AF_INET;
    endServ.sin_addr.s_addr = inet_addr(localhost);
    endServ.sin_port = htons(port);

    /* Fazendo um bind na porta local do servidor */
    if (should_bind) {
        rc = bind(sd, (const struct sockaddr *)&endServ, sizeof(endServ));
        if (rc < 0)
        {
            printf("Conexão com o Cliente falhou\n");
            exit(1);
        }
    }
}

int init_queue(int queue_id)
{
    // ftok to generate unique key

    // msgget creates a message queue
    // and returns identifier
    int tmp = msgget(queue_id, 0666 | IPC_CREAT | IPC_NOWAIT);
    if (tmp < 0) {
        perror("Error initing queue!");
        exit(1);
    }
    return tmp;
}

void slice_str(const char *str, char *buffer, int start, int end)
{
    int j = 0;
    for (int i = start; i < end; ++i)
    {
        buffer[j++] = str[i];
    }
    buffer[j] = 0;
}

void receive_message(struct mesg_buffer* message)
{
    /* Recebe a mensagem  */
    memset(message->mesg_text, 0x0, MAX);
    if (should_bind) {
        int tam_Cli = sizeof(endCli);
        int n = recvfrom(sd, message->mesg_text, MAX, 0, (struct sockaddr *)&endCli, &tam_Cli);
        if (n < 0)
        {
            perror("Error reading udp");
            exit(1);
        }
    } else {
        int tam_Cli = sizeof(endServ);
        int n = recvfrom(sd, message->mesg_text, MAX, 0, (struct sockaddr *)&endServ, &tam_Cli);
        if (n < 0)
        {
            perror("Error reading udp");
            exit(1);
        }
    }
}
int port;
void *sendFile(void *vargp)
{
    int queue_id = *((int *)vargp);
    struct mesg_buffer message;
    message.mesg_type = 1;

    while (1)
    {
        while (1)
        {
            receive_message(&message);

            if (!strcmp(message.mesg_text, "@#\0"))
                break;

            printf("\nReceived package from client: [%s]\n", message.mesg_text);

            int msgid = init_queue(queue_id);

            int n = msgsnd(msgid, &message, sizeof(message), 0);
            printf("\nPackage sent to the queue: [%s]\n", message.mesg_text);

            if (n < 0)
            {
                perror("Error writing on queue");
                exit(1);
            }
        }

        printf("\nEverything sent! ;)\n\n");

        //sleep(5);
    }
}

void *readMessages(void *vargp)
{
    int queue_id = *((int *)vargp);
    char complete_message[100000];
    struct msqid_ds buf;
    struct mesg_buffer message;

    while (1)
    {
        int msgid = init_queue(queue_id);

        int hasRead = 0;

        memset(complete_message, 0, sizeof(complete_message));
        printf("%s\n", complete_message);
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

        int packages_count = 0;

        int message_size = strlen(complete_message);
        int total_packages = message_size / MAX + (message_size % MAX != 0 ? 1 : 0);

        for (int i = 0; i < message_size; i += MAX)
        {
            char buffer[MAX];
            slice_str(complete_message, buffer, i, i + MAX);
            send_message(buffer);

            printf("\nPackage [%d/%d]: [%s] \n", ++packages_count, total_packages, buffer);
        }

        char buffer[3];
        buffer[0] = '@';
        buffer[1] = '#';
        buffer[2] = 0;
        send_message(buffer);

        printf("\nEverything sent! ;)\n\n");
    }
}
int main(int argc, char* argv[])
{
    pthread_t thread_sendFile_id, thread_readMessages_id;
    int send_id = atoi(argv[1]);
    int receive_id = atoi(argv[2]);
    should_bind = atoi(argv[3]);
    init_udp(PORT);
    pthread_create(&thread_readMessages_id, NULL, readMessages, (void *) &receive_id);
    pthread_create(&thread_sendFile_id, NULL, sendFile, (void *) &send_id);
    pthread_join(thread_readMessages_id, NULL);
    pthread_join(thread_sendFile_id, NULL);
    return 0;
}