#include "service.h"
#include "allocationsManager.h"

#define MAX_SERVERS 200
#define MAX_CLIENTS 200
#define GAME_LIST_PATH "json/gameslist.json"
#define MAPS_LIST_PATH "json/mapslist.json"
#define GAME_CREATE_PATH "json/gamecreate.json"

int port = 0;
int num_clients = 0;
char buffer[1024];

/** @brief Signal handler to free allocations when SIGINT on the main server */
void kill_handler(int n)
{
    free_all_allocations();
    exit(EXIT_SUCCESS);
}

/** @brief Function called when client send `POST game/create` request.
 *  @return -1 when in error case (using the `handle_error` MACRO). 0 when no errors
 */
int actionGameCreate(int *cFd)
{
    FILE *gamesListFile = fopen(GAME_CREATE_PATH, "r++");
    if (!gamesListFile)
        handle_error("fopen", -1);

    // BUG: Make segfault when CTRL+C
    /*if (add_file(gamesListFile) < 0)
        handle_error("add_file", -1);*/

    fseek(gamesListFile, 0, SEEK_END);
    long tell = ftell(gamesListFile);
    fseek(gamesListFile, 0, SEEK_SET);

    char content[tell];
    if(fread(content, 1, tell, gamesListFile) < 0) {
        handle_error("fread actionGameCreate", -1);
    }

    content[tell] = '\0';
    send(*cFd, content, strlen(content), 0);

    /*char content[strlen(buffer)];
    for (size_t i = 0; i < strlen(buffer) - 17; i++)
        content[i] = buffer[i + 17];
    printf("%s\n", content);
    cJSON *root = cJSON_Parse(content);

    if (root == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
            handle_error_noexit("Error\n");
        cJSON_Delete(root);
        return 1;
    }

    printf("%s\n", content);
    send(*cFd, content, strlen(content), 0);

    // access the JSON data
    cJSON *name = cJSON_GetObjectItemCaseSensitive(root, "name");
    if (cJSON_IsString(name) && (name->valuestring != NULL))
    {
        printf("Name: %s\n", name->valuestring);
        send(*cFd, name->valuestring, strlen(name->valuestring), 0);
    }

    cJSON_Delete(root);*/
    return 0;
}

/**
 * @brief Function called when client uses command `GET game/list`.
 * @return -1 when in error case (using the `handle_error` MACRO). 0 when no errors
 */
int actionGameList(int *cFd)
{
    FILE *gamesListFile = fopen(GAME_LIST_PATH, "r");
    if (!gamesListFile)
        handle_error("fopen", -1);

    // BUG: Make segfault when CTRL+C
    /*if (add_file(gamesListFile) < 0)
        handle_error("add_file", -1);*/

    fseek(gamesListFile, 0, SEEK_END);
    long tell = ftell(gamesListFile);
    fseek(gamesListFile, 0, SEEK_SET);

    char content[tell];

    if (fread(content, 1, sizeof(content), gamesListFile) < 0)
        handle_error("fread", -1);
    fclose(gamesListFile);

    content[tell] = '\0';
    send(*cFd, content, strlen(content), 0);

    return 0;
}

/**
 * @brief Function called when client uses command `GET maps/list`.
 * @return -1 when in error case (using the `handle_error` MACRO). 0 when no errors
 */
int actionMapsList(int *cFd)
{
    FILE *mapsListFile = fopen(MAPS_LIST_PATH, "r");
    if (!mapsListFile)
        handle_error("fopen", -1);

    // BUG: Make segfault when CTRL+C
    /*if (add_file(gamesListFile) < 0)
        handle_error("add_file", -1);*/

    fseek(mapsListFile, 0, SEEK_END);
    long tell = ftell(mapsListFile);
    fseek(mapsListFile, 0, SEEK_SET);

    char content[tell];

    if (fread(content, 1, sizeof(content), mapsListFile) < 0)
        handle_error("fread", -1);

    fclose(mapsListFile);

    content[tell] = '\0';
    send(*cFd, content, strlen(content), 0);

    return 0;
}

/**
 * @brief Function used to setup the main Server. This will instantiate
 * using `service.h` functions to bind the socket.
 * Then `setupServerManager()` put the given server into listening mode.
 * @param serverManager `struct Server` pointing to the server to setup.
 * @return -1 when in error case (using the `handle_error` MACRO). 0 when no errors
 */
int setupServerManager(Server *serverManager)
{
    if (init_server(serverManager, 42069) < 0)
        handle_error("init_server", -1);

    if (add_server(serverManager) < 0)
        handle_error("add_server", -1);

    if (listen(serverManager->serverSocket, MAX_CLIENTS) < 0)
        handle_error("serverManager: listen", -1);

    printf("I'm listening...\n");

    return 0;
}

void answerServer(int *client_sockets, int index, char *response, char *buffer) {
    if (strncmp(buffer, "POST game/create", 16) == 0)
    {
        if (actionGameCreate(&client_sockets[index]) < 0)
            handle_error_noexit("POST game/create");
    }
    else if (strncmp(buffer, "GET maps/list", 13) == 0)
    {
        if (actionMapsList(&client_sockets[index]) < 0)
            handle_error_noexit("GET maps/list");
    }
    else if (strncmp(buffer, "GET game/list", 13) == 0)
    {
        if (actionGameList(&client_sockets[index]) < 0)
            handle_error_noexit("GET game/list");
    }
    else if (strncmp(buffer, "looking for bomberstudent servers", 33) == 0)
    {

        sprintf(response, "hello i'm a bomberstudent server.\n");
        response[strlen(response) + 1] = '\0';
        send(client_sockets[index], response, strlen(response), 0);
    }
    else
    {
        sprintf(response, "Unkowned command.\n"
                            "List of commands:\n"
                            " - 'looking for bomberstudent servers'\n"
                            " - 'GET maps/list'\n"
                            " - 'GET game/list'\n"
                            " - 'POST game/create'\n");
        response[strlen(response) + 1] = '\0';
        send(client_sockets[index], response, strlen(response), 0);
    }
}

int main(int argc, char *argv[])
{
    signal(SIGINT, kill_handler);
    Server serverManager;
    port = 42069;

    int client_sockets[MAX_CLIENTS];
    int new_socket;
    struct sockaddr_in cAddr;
    socklen_t cAddr_len = sizeof(cAddr);

    fd_set read_fds;

    if (add_string(buffer) < 0)
        handle_error("buffer: addstring", -1);

    int activity, index, valread;

    for (index = 0; index < MAX_CLIENTS; index++)
        client_sockets[index] = 0;

    if (setupServerManager(&serverManager) < 0)
        handle_error("serverManager: setupServerManager", -1);

    for (;;)
    {
        FD_ZERO(&read_fds);
        FD_SET(serverManager.serverSocket, &read_fds);
        int max_sd = serverManager.serverSocket;

        for (index = 0; index < MAX_CLIENTS; index++) // Add clients to the read_fds
        {
            int sd = client_sockets[index];

            if (sd > 0)
                FD_SET(sd, &read_fds);

            if (sd > max_sd)
                max_sd = sd;
        }

        // Await client activity
        activity = select(max_sd + 1, &read_fds, NULL, NULL, NULL);
        if (activity < 0)
            handle_error("activity: select", -1);

        // Accept all new connexions detected
        if (FD_ISSET(serverManager.serverSocket, &read_fds))
        {
            if ((new_socket = accept(serverManager.serverSocket, (struct sockaddr *)&cAddr, (socklen_t *)&cAddr_len)) < 0)
                handle_error("new_socket: accept", -1);

            // Add the new client to the client_sockets group
            for (index = 0; index < MAX_CLIENTS; index++)
            {
                if (client_sockets[index] == 0)
                {
                    num_clients++;
                    client_sockets[index] = new_socket;
                    printf("New connection, socket fd is %d\n", new_socket);
                    break;
                }
            }
        }

        // Check for incoming messages
        for (index = 0; index < MAX_CLIENTS; index++)
        {
            int sd = client_sockets[index];

            if (FD_ISSET(sd, &read_fds))
            {
                if ((valread = read(sd, buffer, sizeof(buffer))) == 0)
                {
                    // Disconnects the client
                    getpeername(sd, (struct sockaddr *)&cAddr, (socklen_t *)&cAddr_len);
                    printf("Host Disconnected, ip %s, port %d\n", inet_ntoa(cAddr.sin_addr), ntohs(cAddr.sin_port));
                    close(sd);
                    client_sockets[index] = 0;
                    num_clients--;
                }
                else
                {
                    char *response = (char *)calloc(256, sizeof(char));
                    buffer[valread] = '\0';
                    // TODO: When received creates a UDP server in a pthread and connects the client fd to it
                    answerServer(client_sockets, index, response, buffer);
                }
            }
        }
    }

    return 0;
}