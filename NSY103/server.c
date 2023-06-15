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

typedef struct
{
    int socket;
    char name[35];
} Client;

typedef struct
{
    int maxClients;
    Client *clientList;
} ServerData;

void *function(void *arg)
{
    ServerData *serverData = (ServerData *)arg;
    Client *client = &(serverData->clientList[serverData->maxClients - 1]);

    char contact[35];
    strcpy(contact, "tu veux parler à qui ? \n");
    if (send(client->socket, contact, sizeof(contact), 0) == -1)
    {
        perror("Error sending contact question");
        close(client->socket);
        pthread_exit(NULL);
    }

    if (recv(client->socket, buffer, sizeof(buffer), 0) <= 0)
    {
        perror("Error receiving contact");  
        close(client->socket);
        pthread_exit(NULL);
    }
    printf("%s veut communiquer avec : %s \n", client->name, buffer);

    // Communication entre les clients
    // Trouver le client cible avec lequel communiquer
    int targetClientIndex = -1;
    for (int i = 0; i < serverData->maxClients; i++)
    {
        if (strcasecmp(buffer, serverData->clientList[i].name) == 0)
        {
            targetClientIndex = i;
            break;
        }
    }

    if (targetClientIndex == -1)
    {
        printf("Target client not found.\n");
        close(client->socket);
        pthread_exit(NULL);
    }

   // Écoute continuellement les messages du client et les envoie au client cible.
    while (1)
    {
        // Réception d'un message du client
        int recvSize = recv(client->socket, buffer, sizeof(buffer), 0);
        if (recvSize <= 0)
        {
            perror("Error receiving message");
            break;
        }

     // Mettre fin à la communication si un signal de fin de communication est reçu
        if (strcmp(buffer, "quit") == 0)
        {
            printf("%s has terminated the communication.\n", client->name);
            break;
        }

       // Envoi d'un message au client
        int targetSocket = serverData->clientList[targetClientIndex].socket;
        if (targetSocket == -1)
        {
            printf("Invalid socket descriptor for target client.\n");
            break;
        }

        if (send(targetSocket, buffer, recvSize, 0) == -1)
        {
            perror("Error sending message");
            break;
        }
    }

    close(client->socket);
    pthread_exit(NULL);
}

int main(void)
{
    int socketServer = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addrServer;
    addrServer.sin_family = AF_INET;
    addrServer.sin_port = htons(1234);
    addrServer.sin_addr.s_addr = inet_addr("192.168.80.20");

    // test création socket
    if (socketServer == -1)
    {
        printf("error socket creation\n");
        return 1;
    }

    // donne au fd l'adresse locale
    if (bind(socketServer, (const struct sockaddr *)&addrServer, sizeof(addrServer)))
    {
        printf("error Giving the socket FD the local address ADDR\n");
        close(socketServer);
        return 1;
    }

    // prepare pour accepter connexion
    // fd, nb de personne en attente
    if (listen(socketServer, 10) == -1)
    {
        printf("error Preparing to accept connections on socket FD\n");
        close(socketServer);
        return 1;
    }

    printf("listen\n");

    int maxClients = 3; // Maximum number of clients
    Client *clientList = malloc(maxClients * sizeof(Client));

    for (int i = 0; i < maxClients; i++)
    {
        struct sockaddr_in addrClient;
        socklen_t csize = sizeof(addrClient);

        int socketClient = accept(socketServer, (struct sockaddr *)&addrClient, &csize);
        printf("accept\n");

        char message[35];
        strcpy(message, "connecté sur le serveur\n"); // %s supprimé
        strcat(message, " Quel est ton pseudo ? \n" );
        if (send(socketClient, message, strlen(message) + 1, 0) == -1)
        {
            perror("Error sending connection message\n");
            close(socketClient);
            continue;
        }

        // Reception du nom du client
        if (recv(socketClient, buffer, sizeof(buffer), 0) <= 0)
        {
            perror("Error receiving client name\n");
            close(socketClient);
            continue;
        }

        printf("%s est connecté\n", buffer);

        Client newClient;
        newClient.socket = socketClient;
        strcpy(newClient.name, buffer);

        clientList[i] = newClient;

        pthread_t clientThread;
        ServerData serverData;
        serverData.maxClients = i + 1;
        serverData.clientList = clientList;

        if (pthread_create(&clientThread, NULL, function, (void *)&serverData) != 0)
        {
            perror("Error creating client thread\n");
            close(socketClient);
            continue;
        }
    }

    free(clientList);
    close(socketServer);
    printf("close\n");
    return 0;
}