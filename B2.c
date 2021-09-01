// C Program for Message Queue (Writer Process)
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

#define localhost "127.0.0.1"
#define PORT 8080
#define MAX 10
#define QUEUE_ID 62

// structure for message queue
struct mesg_buffer
{
    long mesg_type;
    char mesg_text[MAX];
} message;

int sd, rc, tam_Cli, n;
char buffer[MAX];
struct sockaddr_in endServ;
struct sockaddr_in endCli;

key_t key;
int msgid;

void receive_message()
{
    /* Recebe a mensagem  */
    memset(message.mesg_text, 0x0, MAX);
    tam_Cli = sizeof(endCli);
    n = recvfrom(sd, message.mesg_text, MAX, 0, (struct sockaddr *)&endCli, &tam_Cli);
}

void init_queue()
{
    msgid = msgget(QUEUE_ID, 0666 | IPC_CREAT | IPC_NOWAIT);
    message.mesg_type = 1;
}

void init_udp()
{
    /* Criacao do socket UDP */
    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sd < 0)
    {
        printf("Ocorreu uma falha na hora de criar o servidor\n");
        exit(1);
    }
    else
    {
        printf("Servidor criado com sucesso\n");
    }

    endServ.sin_family = AF_INET;
    endServ.sin_addr.s_addr = inet_addr(localhost);
    endServ.sin_port = htons(PORT);

    /* Fazendo um bind na porta local do servidor */
    rc = bind(sd, (const struct sockaddr *)&endServ, sizeof(endServ));
    if (rc < 0)
    {
        printf("Conexão com o Cliente falhou\n");
        exit(1);
    }
}

int main()
{

    init_udp();

    while (1)
    {
        system("clear");
        printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
        printf("@@           Link B2            @@\n");
        printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");

        printf("\nWaiting for messages via UDP socket...\n\n");

        while (1)
        {
            init_queue();

            receive_message();

            if (!strcmp(message.mesg_text, "@#\0"))
                break;

            printf("\nReceived package from client: [%s]\n", message.mesg_text);

            int n = msgsnd(msgid, &message, sizeof(message), 0);
            printf("\nPackage sent to the queue: [%s]\n", message.mesg_text);

            if (n < 0)
            {
                perror("Error writing on queue");
                exit(1);
            }
        }

        printf("\nEverything sent! ;)\n\n");

        sleep(5);
    }

    return 0;
}