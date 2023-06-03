#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>


#define BUFFER_LEN 1024

// typedef struct CLIENT{
//     char NOM[30];
//     int age;
//     int id;

// }CLIENT;

int main(void)
{
    char buffer[BUFFER_LEN];
    int socketClient = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addrClient;
    addrClient.sin_family = AF_INET;
    addrClient.sin_port = htons(1234);
    addrClient.sin_addr.s_addr = inet_addr("192.168.80.20");

    // test création socket
    if (socketClient == -1)
    {
        printf("error socket creation\n");
    }

    // CLIENT Client;

    // connexion du client
    if (connect(socketClient, (struct sockaddr *)&addrClient, sizeof(addrClient)) == -1)
    {
        printf("error Openning a connection on socket FD to peer at ADDR");
    }

   
    recv(socketClient, buffer, sizeof(buffer), 0);
    printf("%s\n", buffer);

    recv(socketClient, buffer, sizeof(buffer), 0);
    printf("%s", buffer);

    scanf("%s", buffer);
    send(socketClient, buffer, sizeof(buffer), 0);

    
    recv(socketClient, buffer, sizeof(buffer), 0);
    printf("%s", buffer);
    scanf("%s", buffer);
    send(socketClient, buffer, sizeof(buffer), 0);

    recv(socketClient, buffer, sizeof(buffer), 0);
    printf("%s", buffer);

    close(socketClient);

    return 0;
}
