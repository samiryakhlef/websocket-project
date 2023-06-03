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
#include <pthread.h>
#include <fcntl.h>

#define BUFFER_LEN 1024
static char buffer[BUFFER_LEN];

void *function(void *arg)
{
    int socket = (int *)arg; //diff
    char pseudo[35];
    strcpy(pseudo,"quel est ton pseudo ?\n" );
    send(socket, pseudo, sizeof(pseudo), 0);
    
    recv(socket, buffer, sizeof(buffer), 0);
    printf("%s est connecté\n", buffer);

    // communiquer avecqui ? 
    char contact[35];
    strcpy(contact,"tu veux parler à qui ? \n");
    send(socket, contact, sizeof(contact), 0);
    recv(socket, buffer, sizeof(buffer), 0);
    printf("demande de connexion pour : %s \n", buffer);

    close(socket);
    pthread_exit(NULL);
}

int main(void)
{
   

    pthread_t clientThread;
    
    int socketServer = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addrServer;
    addrServer.sin_family = AF_INET;
    addrServer.sin_port = htons(1234);
    addrServer.sin_addr.s_addr = inet_addr("192.168.80.20");
    
    // test création socket
    if (socketServer == -1) {
        printf("error socket creation\n");
        return 1;
    }
    
    // donne au fd l'adresse locale
    if (bind(socketServer, (const struct sockaddr *)&addrServer, sizeof(addrServer))) {
        printf("error Giving the socket FD the local address ADDR\n");
        close(socketServer);
        return 1;
    }
    
    // prepare pour accepter connexion
    // fd, nb de personne en attente
    if (listen(socketServer, 10) == -1) {
        printf("error Preparing to accept connections on socket FD\n");
        close(socketServer);
        return 1;
    }
    
    printf("listen\n");
    
    pthread_t threadClient[3];
    
    for (int i = 0; i < 3; i++) {
        struct sockaddr_in addrClient;
        socklen_t csize = sizeof(addrClient);
        
        int socketClient = accept(socketServer, (struct sockaddr *)&addrClient, &csize);
        printf("accept\n");
        
        //char message[100] = "connecté sur le serveur ROCKY";
        //send(socketClient, message, sizeof(message), 0);
        char message[35];
        strcpy(message, "connecté sur le serveur ROCKY");
        send(socketClient, message, sizeof(message), 0);
        
        pthread_create(&threadClient[i], NULL, function, (void *)socketClient);
    }
    
    for (int i = 0; i < 3; i++) {
        pthread_join(threadClient[i], NULL);
    }
    
    close(socketServer);
    printf("close\n");
    return 0;
}
