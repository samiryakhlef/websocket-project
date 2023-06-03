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
#include <semaphore.h>

#define BUFFER_LEN 1024
static char buffer[BUFFER_LEN];
// Création du sémaphore;
sem_t semaphore;

// tableau de client connectés
struct CLIENT *ClientConnect[3];
int nbrConnect = 0;

struct CLIENT
{
    int id;
    char pseudo[10];
    int socketClient;

} *Client;

void *function(void *Client)
{
    struct CLIENT *client = (struct CLIENT *)Client;
    // int socket = (int *)arg; //diff
    char pseudo[35];
    strcpy(pseudo, "quel est ton pseudo ?\n");
    send(client->socketClient, pseudo, sizeof(pseudo), 0);

    recv(client->socketClient, buffer, sizeof(buffer), 0);
    printf("%s est connecté\n", buffer);
    nbrConnect++;
    printf("%d sont connectés\n ", nbrConnect);

    strcpy(client->pseudo, buffer);
    ClientConnect[client->id] = client;
    printf("%s dans tableau des clients connectés\n", ClientConnect[client->id]->pseudo);

    // communiquer avec qui ?
    char contact[35];
    strcpy(contact, "tu veux parler à qui ? \n");
    send(client->socketClient, contact, sizeof(contact), 0);
    recv(client->socketClient, buffer, sizeof(buffer), 0);

    char pseudoRecu[35];

    strcpy(pseudoRecu, buffer);
    printf("demande de connexion pour : %s \n", pseudoRecu);
    // Gérer la demande de connexion
    // si connecter anvoyer un message
    // au pseudo en question

    // boucle sur le nbr de client connectés

    int clientTrouve = 0;

    for (int i = 0; i < nbrConnect; i++)
    {
        //printf("%s est dispo\n", ClientConnect[i]->pseudo);

        if (strcmp(buffer, ClientConnect[i]->pseudo) == 0)
        {
            char connexion[35];
            strcpy(connexion, "init connexion avec ");
            strcat(connexion, client->pseudo);
            // send(ClientConnect[i]->socketClient, connexion, sizeof(connexion), 0);
            clientTrouve = 1;
            break;
        }
    }

    if (!clientTrouve)
    {
        char message[105];
        strcpy(message, "Aucun utilisateur avec le pseudo spécifié n'est disponible\n");
        send(client->socketClient, message, sizeof(message), 0);
    }

    // //si connexion accepté on entre dans le semaphore
    // while (1) {
    // 	// On attend la disponibilité du sémaphore
    // 	sem_wait(&semaphore);
    // 	// Section critique
    // 	printf("Je suis le thread [%i] et je vais dormir 1 seconde\n", tid);
    // 	sleep(1);
    // 	printf("Je suis le thread [%i] et j'ai fini ma sieste\n", tid);
    // 	// On relache le sémaphore
    // 	sem_post(&semaphore);

    // }

    close(client->socketClient);
    pthread_exit(EXIT_SUCCESS);
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
    if (listen(socketServer, 1) == -1)
    {
        printf("error Preparing to accept connections on socket FD\n");
        close(socketServer);
        return 1;
    }

    printf("listen\n");

    // tableau de thread
    pthread_t threadClient[3];

    // Initialisation du sémaphore
    sem_init(&semaphore, PTHREAD_PROCESS_SHARED, 1);

    for (int i = 0; i < 3; i++)
    {
        ClientConnect[i] = NULL;
    }

    for (int i = 0; i < 3; i++)
    {
        struct sockaddr_in addrClient;
        socklen_t csize = sizeof(addrClient);
        ClientConnect[i] = malloc(sizeof(struct CLIENT));

        int socketClient = accept(socketServer, (struct sockaddr *)&addrClient, &csize);
        printf("accept\n");

        // allocate a separate pair for each thread
        Client = malloc(sizeof(struct CLIENT));
        Client->socketClient = socketClient;
        Client->id = i;

        // char message[100] = "connecté sur le serveur ROCKY";
        // send(socketClient, message, sizeof(message), 0);
        char message[35];
        strcpy(message, "connecté sur le serveur ROCKY");
        send(socketClient, message, sizeof(message), 0);

        // pthread_create(&threadClient[i], NULL, function, (void *)socketClient);
        pthread_create(&threadClient[i], NULL, function, (void *)Client);
    }

    for (int i = 0; i < 3; i++)
    {
        pthread_join(threadClient[i], NULL);
    }
    sem_destroy(&semaphore);
    close(socketServer);
    printf("close\n");
    return 0;
}
