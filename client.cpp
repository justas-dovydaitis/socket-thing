#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

#define PORT 54000
#define BUFFER_SIZE 4096

char *make_command(int argc, char **argv)
{
    int size = 0;
    int arguments = argc - 1;

    char *command = (char *)malloc(0);

    for (int i = 1; i <= arguments; i++)
    {
        command = (char *)realloc(command, (size + strlen(argv[i]) + 1));
        strcat(command, argv[i]);
        strcat(command, " ");
    }
    return command;
}

int main(int argc, char **argv)
{
    int network_socket;
    network_socket = socket(AF_INET, SOCK_STREAM, 6);
    if (network_socket == -1)
    {
        printf("Error creating socket\n");
        exit(-1);
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    auto connection_status = connect(network_socket, (sockaddr *)&server_address, sizeof(server_address));
    if (connection_status == -1)
    {
        printf("Error making a connection to the remote server\n");
        exit(-1);
    }

    int message_size = 0;
    const int arguments = argc - 1;

    char *message = make_command(argc, argv);

    send(network_socket, message, strlen(message), 0);

    char server_response[BUFFER_SIZE];
    recv(network_socket, &server_response, sizeof(server_response), 0);

    printf("Response received:\n\n %s\n", server_response);
    close(network_socket);

    return 0;
}