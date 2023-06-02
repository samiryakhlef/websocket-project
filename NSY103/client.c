#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <arpa/inet.h>

#define PORT 1234

int main(void)
{
    int client_socket;
    struct sockaddr_in server_address;
    char recvBuffer[1000], sendBuffer[1000];

    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("192.168.112.1");
    server_address.sin_port = htons(1234);

    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        perror("Connect failed");
        exit(EXIT_FAILURE);
    }

    printf("Enter message: ");
    fgets(sendBuffer, sizeof(sendBuffer) - 1, stdin);

    // Send the message
    if (send(client_socket, sendBuffer, strlen(sendBuffer), 0) == -1)
    {
        perror("Send failed");
        exit(EXIT_FAILURE);
    }

    // Receive the response
    if (recv(client_socket, recvBuffer, sizeof(recvBuffer), 0) == -1)
    {
        perror("Receive failed");
        exit(EXIT_FAILURE);
    }

    printf("Server response: %s\n", recvBuffer);

    close(client_socket);

    return 0;
}