#ifndef SERVICE_H
#define SERVICE_H

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>

/**
 * @brief `int` Macro printing an error message and returning your error status
 * @param msg `char *` perror your message passed
 * @param status `int` return the value for error handling
 * @returns exit (status)
 */
#define handle_error(msg, status) \
    do                            \
    {                             \
        perror(msg);              \
        return status;            \
    } while (0)

/**
 * @brief Macro printing and error message without exiting
 * @param msg perror your message passed
 */
#define handle_error_noexit(msg) \
    do                           \
    {                            \
        perror(msg);             \
    } while (0)

/**
 * @brief Structure for each Server
 * @param server_socket `int` Server File Descriptor used for the socket
 * @param server_addr `sockaddr_in` An instance of the sockaddr_in struct
 * @param protocol `char *` Set to `t` if it's a TCP server or `u` if it's a UDP server
 */
typedef struct
{
    int server_socket;
    struct sockaddr_in server_addr;
    char *protocol;
} Server;

/**
 * @brief Function used for the socket initialisation
 * @param server: `Server *` Pointer to the server struct
 * @param port: `int` port on which the server will be listening to
 */
int init_server(Server *server, int port, int type);

#endif