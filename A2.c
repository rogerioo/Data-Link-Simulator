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

#define localhost "127.0.0.1"
#define QUEUE_ID 65
#define PORT 8080
#define MAX 10

// structure for message queue
struct mesg_buffer
{
    long mesg_type;
    char mesg_text[100];
} message;

int sd;
struct sockaddr_in endServ;

int msgid;
struct msqid_ds buf;

char complete_message[100000];

void send_message(const char *msg)
{
    /* Envia a mensagem  */

    printf("Enviando: %s\n", msg);
    int n = sendto(sd, msg, strlen(msg), 0,
                   (const struct sockaddr *)&endServ, sizeof(endServ));
    if (n < 0)
    {
        perror("Erro ao enviar mensagem");
        exit(1);
    }
}

void init_udp()
{
    endServ.sin_family = AF_INET;
    endServ.sin_port = htons(PORT);
    endServ.sin_addr.s_addr = inet_addr(localhost);

    /* Criando um socket.*/
    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sd < 0)
    {
        printf("Conexão com o Servidor falhou\n");
        exit(1);
    }
}

void init_queue()
{
    // ftok to generate unique key

    // msgget creates a message queue
    // and returns identifier
    msgid = msgget(QUEUE_ID, 0666 | IPC_CREAT | IPC_NOWAIT);
}

void empty_message()
{
    memset(complete_message, 0x0, strlen(complete_message));
}

void slice_str(const char *str, char *buffer, int start, int end)
{
    int j = 0;
    for (int i = start; i < end; ++i)
    {
        // printf("@ %d %d %d\n", start, i, end);
        buffer[j++] = str[i];
    }
    buffer[j] = 0;
}

int main()
{
    int hasRead = 0;
    init_queue();
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
            printf("oie\n");
            printf("%s\n", complete_message);
            break;
        }
        msgrcv(msgid, &message, sizeof(message), 1, 0);
        strcat(complete_message, message.mesg_text);
        hasRead = 1;
        sleep(0.5);
    }
    printf("saiu\n");
    init_udp();

    for (int i = 0; i < strlen(complete_message); i += MAX)
    {
        char buffer[MAX];
        slice_str(complete_message, buffer, i, i + MAX);
        send_message(buffer);
    }

    msgctl(msgid, IPC_RMID, NULL);

    return 0;
}