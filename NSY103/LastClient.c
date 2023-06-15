#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define SERVER_IP "192.168.80.20"
#define PORT 1234
char buffer[1024];
int read_size;
int close_socket = 0;

void *receive_messages(void *arg)
{
    int client_socket = *(int *)arg;

    while (1)
    {
        read_size = recv(client_socket, buffer, sizeof(buffer), 0);
        if (read_size == -1)
        {
            perror("recv failed\n");
            break;
        }

        if (read_size == 0)
        {
            printf("Disconnected from server\n");
            break;
        }
        buffer[read_size] = '\0';

        printf("Received message: %s\n", buffer);
    }

    pthread_exit(NULL);
}

void start_client()
{
    int client_socket;
    struct sockaddr_in server_addr;
    char message[1024];
    pthread_t recv_thread;

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1)
    {
        perror("socket creation failed\n");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, SERVER_IP, &(server_addr.sin_addr)) <= 0)
    {
        perror("invalid address / address not supported\n");
        exit(EXIT_FAILURE);
    }

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("connection failed\n");
        exit(EXIT_FAILURE);
    }

    if (pthread_create(&recv_thread, NULL, receive_messages, (void *)&client_socket) != 0)
    {
        perror("thread creation failed\n");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        printf("Enter a message (or 'exit' to quit): \n");
        fgets(message, sizeof(message), stdin);
        message[strcspn(message, "\n")] = '\0';

        if (strcmp(message, "exit") == 0)
        {
            close(client_socket);
            close_socket = 1;
            
        }

        if (send(client_socket, message, strlen(message), MSG_DONTWAIT) < 0 && close_socket ==1)
        {
            printf("erreur envoie\n");
            break;
        };
        close_socket = 0;
    }

    close(client_socket);
    pthread_cancel(recv_thread);
    pthread_join(recv_thread, NULL);
}

int main()
{
    start_client();
    return 0;
}