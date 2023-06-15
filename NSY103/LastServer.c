#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <syslog.h>

#define BUFFER_LEN 1024
static char buffer[BUFFER_LEN];
static char bufferSignal[10];
int pb = 0;
//int deconnect = 0;

typedef struct
{
    int socket;
    char name[35];
    int deconnect;
} Client;

typedef struct
{
    int maxClients;
    Client *clientList;
    FILE *logFile;
} ServerData;

int maxClients = 3; // Nombre maximum de clients
// Client *clientList = malloc(maxClients * sizeof(Client));
Client clientList[3];
int numClients = 0;

int end = 0;
int signalId;

FILE *file;

void logEvent(const char *message)
{
    openlog("LastServer", LOG_PID | LOG_CONS, LOG_USER);
    syslog(LOG_INFO, message);
    closelog();
    // file = fopen("server_logs.txt", "a");
    //  if (file == NULL)
    //  {
    //      perror("Erreur lors de l'ouverture du fichier de logs");
    //      return;
    //  }

    // time_t currentTime;
    // time(&currentTime);
    // char *timeString = ctime(&currentTime);
    // timeString[strlen(timeString) - 1] = '\0'; // Supprimer le caractère de nouvelle ligne

    // fprintf(file, "[%s] %s\n", timeString, message);

    // fclose(file);
}

void signal_handler(int signum)
{
    // sighup : 1, sigint : 2, sigterm : 15
    if (signum == SIGHUP)
    { // Si le signal est SIGHUP, alors on redémarre le processus
      // execvp remplace le processus courant par un nouveau processus
      // Le premier argument est le nom du programme à exécuter
      // Le second argument est un tableau de chaînes de caractères qui représentent les arguments à passer au programme
        logEvent("Signal de redémarrage reçu reçu, arrêt du serveur");
        execvp("./LastServer", NULL);
        end = 1;
    }
    else if (signum == SIGTERM || signum == SIGINT)
    { // Si le signal est SIGTERM ou SIGINT, alors on arrête le processus
        // Écrit un message de log pour signaler que le serveur va s'arrêter
        logEvent("Signal de terminaison reçu, arrêt du serveur");
        // Ferme le fichier de log et le socket du serveur
        // fclose(file);
        printf("dans signal");
        for (int i = 0; i < maxClients; i++)
        {
            char message_server[35] = "fermeture du server";
            if (send(clientList[i].socket, message_server, sizeof(message_server), 0) < 0)
            {
                printf("problème envoie message fermetyure du server");
            }
            close(clientList[i].socket);
        }

        // Arrête le processus avec le code de sortie 1
        // exit(1);
        end = 1;
    }
}

void init_client()
{
    for (int i = 0; i < maxClients; i++)
    {
        clientList[numClients].socket = 0;
        strcpy(clientList[numClients].name, "");
        clientList[numClients].deconnect = 0;

    }
}

void verif_client(Client *client)
{
    // Vérification si le nom du client est unique
    int isNameUnique = 0;
    for (int i = 0; i < numClients; i++)
    {
        if (strcmp(client->name, clientList[i].name) == 0)
        {
            isNameUnique++;
        }
    }

    if (isNameUnique > 1)
    {
        char error[] = "Ce nom est déjà utilisé. Déconnexion...\n";
        if (send(client->socket, error, sizeof(error), 0) == -1)
        {
            perror("Ce nom est déjà utilisé. Déconnexion...");
            // fprintf(logFile, "Erreur lors de l'envoi du message de nom déjà utilisé.\n");
            logEvent("Ce nom est déjà utilisé. Déconnexion...");
        }
        close(client->socket);
    }
}

void *handle_client(void *arg)
{
    Client *client = (Client *)arg;

    char message[] = "Connecté au serveur :\n";

    if (send(client->socket, message, sizeof(message), 0) == -1)
    {
        perror("Erreur lors de l'envoi du message de connexion");
        // fprintf(logFile, "Erreur lors de l'envoi du message de connexion.\n");
        logEvent("Erreur lors de l'envoi du message de connexion");
        close(client->socket);
    }

    // Réception du nom du client
    char nameBuffer[35];
    char prompt[] = "Entrez votre nom : ";
    
    if (send(client->socket, prompt, sizeof(prompt), 0) == -1)
    {
        perror("Erreur lors de l'envoi de la demande de nom");
        // fprintf(logFile, "Erreur lors de l'envoi de la demande de nom.\n");
        logEvent("Erreur lors de l'envoi de la demande de nom");
        close(client->socket);
    }

    int recvSize = recv(client->socket, nameBuffer, sizeof(nameBuffer), 0);
    if (recvSize <= 0)
    {
        perror("Erreur lors de la réception du nom du client");
        // fprintf(logFile, "Erreur lors de la réception du nom du client.\n");
        logEvent("Erreur lors de la réception du nom du client");
        close(client->socket);
    }
    nameBuffer[recvSize] = '\0';
    strcpy(client->name, nameBuffer);

    verif_client(client);

    while (!pb)
    {

        int recvSize = recv(client->socket, buffer, sizeof(buffer), 0);
        if (recvSize == 0 && errno != EAGAIN)
        {
            printf("%s s'est déconnecté.\n", client->name);
            char mess_error[35] = "Déconnecté  : ";
            logEvent(strcat(mess_error, client->name));
            //close(client->socket);
            client->deconnect = 1;
            break;
        }

       
            // Diffusion du message reçu aux autres clients
            for (int i = 0; i < maxClients; i++)
            {
                if (clientList[i].socket > 0 && clientList[i].deconnect == 0)
                {
                    if (send(clientList[i].socket, buffer, recvSize, 0) < 0)
                    {
                        perror("Erreur lors de l'envoie du message");
                        // fprintf(logFile, "Erreur lors de l'envoi du message.\n");
                        logEvent("Erreur lors de l'envoi du message");
                        // pb = 1;
                        break;
                    }
                }
            }
        
    }
    close(client->socket);
    pthread_exit(NULL);
}

int main(void)
{

    char pid[10];
    sprintf(pid, "%d", getpid());
    logEvent(pid);

    // signal(SIGINT, &sigInt);
    // signal(SIGHUP, &sigHup);

    if (signal(SIGINT, signal_handler) == SIG_ERR || (signal(SIGHUP, signal_handler) == SIG_ERR || (signal(SIGTERM, signal_handler) == SIG_ERR)))
    {
        logEvent("Erreur à l'enregistrement du gestionnaire de signaux");
    };

    daemon(0, 0);

    int socketServer = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addrServer;
    addrServer.sin_family = AF_INET;
    addrServer.sin_port = htons(1234);
    addrServer.sin_addr.s_addr = inet_addr("192.168.80.20");

    // Test de création de socket
    if (socketServer == -1)
    {
        perror("Erreur lors de la création de la socket");
        logEvent("Erreur lors de la création de la socket");
        return 1;
    }
    int opt = 1;
    if (setsockopt(socketServer, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) != 0)
    {
        perror("Erreur lors de la liaison de la socket");
        logEvent("Erreur lors de la liaison de la socket");
        close(socketServer);
        return 1;
    }

    // Lier la socket à une adresse locale
    if (bind(socketServer, (struct sockaddr *)&addrServer, sizeof(addrServer)) == -1)
    {
        perror("Erreur lors de la liaison de la socket");
        logEvent("Erreur lors de la liaison de la socket");
        close(socketServer);
        return 1;
    }

    // Écoute des connexions
    if (listen(socketServer, 10) == -1)
    {
        perror("Erreur lors de l'écoute des connexions");
        logEvent("Erreur lors de l'écoute des connexions");
        close(socketServer);
        return 1;
    }

    logEvent("Le serveur écoute les connexions...\n");

    init_client();

    while (!end)
    {

        struct sockaddr_in addrClient;
        socklen_t csize = sizeof(addrClient);

        int socketClient = accept(socketServer, (struct sockaddr *)&addrClient, &csize);
        if (socketClient == -1)
        {
            perror("Erreur lors de l'acceptation de la connexion");
            logEvent("Erreur lors de l'écoute des connexions");
            continue;
        }

        printf("Un nouveau client est connecté :)\n");
        logEvent("Un nouveau client est connecté");

        // tester si le tableau est un espace est vide si oui affecter le nouveau client connecté
        clientList[numClients].socket = socketClient;

        // Créer un nouveau thread pour gérer le client
        pthread_t clientThread;
        if (pthread_create(&clientThread, NULL, handle_client, (void *)&clientList[numClients]) != 0)
        {
            perror("Erreur lors de la création du thread client");
            logEvent("Erreur lors de la création du thread client");
            close(socketClient);
            continue;
        }
        numClients++;
    }
    close(socketServer);
    fclose(file);
    return 0;
}
