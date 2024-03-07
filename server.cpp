/*
** server.c -- a stream socket server demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

// colored print outputs
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define END "\033[0m"

#define PORT "3490" // the port users will be connecting to

#define BACKLOG 10 // how many pending connections queue will hold
#define MAXDATASIZE 100

//////////////////////// STRUCT
struct ClientTopics
{
    int pid;
    int topic_size = 0;
    char **topics;
};

//////////////////////// FUNC
struct ClientTopics *findPid(struct ClientTopics *arr, int size)
{
    for (int i = 0; i < size; i++)
    {
        if (arr[i].pid == 5)
        {
            return &arr[i];
        }
    }
    return NULL; // Return NULL if num 5 is not found
}

void print_err(const char *msg)
{
    printf("%s%s%s\n", RED, msg, END);
}

void print_success(const char *msg)
{
    printf("%s%s%s\n", GREEN, msg, END);
}

void print_help(const char *msg)
{
    printf("%s%s%s\n", YELLOW, msg, END);
}

void client_disconnect(char *str_contain, int new_fd)
{
    if (str_contain != NULL)
    {
        if (send(new_fd, "DISCONNECT", 10, 0) == -1)
        {

            perror("send");
        }
        print_success("Closing connection");
    }
}

int check_message_len(char *buf)
{
    if (strlen(buf) == 0)
    {
        print_err("Empty message received, closing connection\n");
        return 1;
    }
    return 0;
}

//////////////////////// TCP IP FUNC
void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while (waitpid(-1, NULL, WNOHANG) > 0)
        ;

    errno = saved_errno;
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(void)
{
    // TCP IP vars
    int sockfd, new_fd; // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes = 1;
    char s[INET6_ADDRSTRLEN];
    int rv;
    int numbytes;
    char buf[MAXDATASIZE];

    // CLIENT COMMANDS
    char disconnect[] = "DISCONNECT";
    char publish[] = "PUBLISH";
    char subscribe[] = "SUBSCRIBE";
    char unsubscribe[] = "UNSUBSCRIBE";

    // when comparing client message with command
    char *str_contain;

    // process pid
    int pid;

    // struct array init
    struct ClientTopics *client_pairs = NULL;

    // malloc for Client topic struct
    client_pairs = (ClientTopics *)malloc(sizeof(struct ClientTopics));
    if (client_pairs == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1)
        {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                       sizeof(int)) == -1)
        {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL)
    {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1)
    {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");

    while (1)
    { // main accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        printf("Starting new connection %d\n", new_fd);
        if (new_fd == -1)
        {
            perror("accept");
            continue;
        }
        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
        printf("server: got connection from %s\n", s);
        // child process
        int retval = fork();
        if (retval != 0)
        {
            // set pid for the new client when it connects
            pid = retval;
        }
        if (!retval)
        {
            print_success("Fork success");
            // set pid for the new client
            client_pairs[0].pid = pid;
            while (true)
            {
                if ((numbytes = recv(new_fd, buf, MAXDATASIZE - 1, 0)) == -1)
                {
                    perror("recv");
                    exit(1);
                }

                buf[numbytes] = '\0';
                printf("%d server: received '%s%s%s'\n", client_pairs[0].pid, GREEN, buf, END);
                if (strlen(buf) == 0)
                {
                    print_err("Empty message received, closing connection\n");
                    break;
                }
                str_contain = strstr(buf, disconnect);
                if (str_contain != NULL)
                {
                    if (send(new_fd, "DISCONNECT", 10, 0) == -1)
                    {
                        perror("send");
                    }
                    print_success("Closing connection");
                    break;
                }

                str_contain = strstr(buf, publish);
                if (str_contain != NULL)
                {

                    char *token;
                    char *data[2];
                    char buf_cpy[strlen(buf) + 1];
                    strcpy(buf_cpy, buf);

                    token = strtok(buf + strlen("PUBLISH") + 1, " ");
                    if (token != NULL)
                    {
                        data[0] = strdup(token);
                        token = strtok(NULL, " ");
                        if (token != NULL)
                        {
                            data[1] = strdup(token);
                            for (int i = 0; i < client_pairs[0].topic_size; i++)
                            {
                                if (strcmp(client_pairs[0].topics[i], data[0]) == 0)
                                {
                                    strcpy(buf_cpy, data[1]);
                                }
                            }
                            free(data[0]);
                            free(data[1]);
                        }
                    }

                    strcpy(buf, buf_cpy);
                }
                str_contain = strstr(buf, unsubscribe);
                if (str_contain != NULL)
                {
                    char *token;
                    char *data[1];

                    token = strtok(buf + strlen("UNSUBSCRIBE") + 1, " ");
                    if (token != NULL)
                    {

                        int index = -1;
                        data[0] = strdup(token);
                        for (int i = 0; i < client_pairs[0].topic_size; i++)
                        {

                            int result = strcmp(client_pairs[0].topics[i], data[0]);
                            if (result == 0)
                                index = i;
                        }
                        if (index != -1)
                        {
                            for (int i = index; i < client_pairs[0].topic_size - 1; i++)
                            {
                                client_pairs[0].topics[i] = (char *)realloc(client_pairs[0].topics[i], strlen(client_pairs[0].topics[i+1]) + 1); // Allocate memory for the string
                                if (client_pairs[0].topics[i] == NULL)
                                {
                                    fprintf(stderr, "Memory allocation failed\n");
                                    free(client_pairs[0].topics);
                                    free(client_pairs);
                                    exit(1);
                                }
                                strcpy(client_pairs[0].topics[i], client_pairs[0].topics[i + 1]);
                            }

                            // Resize the array
                            client_pairs[0].topics = (char **)realloc(client_pairs[0].topics, (client_pairs[0].topic_size) * sizeof(char *));
                            if (client_pairs[0].topics == NULL && client_pairs[0].topic_size > 1)
                            {
                                // Memory reallocation failed, but original array still exists
                            }
                            client_pairs[0].topic_size = client_pairs[0].topic_size - 1;
                        }
                        printf("Client subscriptions: ");
                        printf("[%d, ", client_pairs[0].pid);
                        for (int i = 0; i < client_pairs[0].topic_size; i++)
                        {
                            if (i == 0)
                                printf("%s", client_pairs[0].topics[i]);
                            else
                                printf(", %s", client_pairs[0].topics[i]);
                            int result = strcmp(client_pairs[0].topics[i], data[0]);
                            if (result == 0)
                                index = i;
                        }
                        printf("]\n");
                    }
                    free(data[0]);
                }
                else
                {
                    str_contain = strstr(buf, subscribe);
                    if (str_contain != NULL)
                    {
                        char *token;
                        char *data[1];

                        token = strtok(buf + strlen("SUBSCRIBE") + 1, " ");
                        if (token != NULL)
                        {
                            data[0] = strdup(token);
                            client_pairs[0].topics = (char **)realloc(client_pairs[0].topics, sizeof(char *) * client_pairs[0].topic_size + 1); // Allocate memory for one string
                            client_pairs[0].topic_size++;
                            if (client_pairs[0].topics == NULL)
                            {
                                fprintf(stderr, "Memory allocation failed\n");
                                free(client_pairs);
                                return 1;
                            }

                            client_pairs[0].topics[client_pairs[0].topic_size - 1] = (char *)realloc(client_pairs[0].topics[client_pairs[0].topic_size - 1], strlen(data[0]) + 1); // Allocate memory for the string
                            if (client_pairs[0].topics[client_pairs[0].topic_size - 1] == NULL)
                            {
                                fprintf(stderr, "Memory allocation failed\n");
                                free(client_pairs[0].topics);
                                free(client_pairs);
                                exit(1);
                            }
                            strcpy(client_pairs[0].topics[client_pairs[0].topic_size - 1], data[0]);
                            printf("Client subscriptions: ");
                            printf("[%d, ", client_pairs[0].pid);
                            for (int i = 0; i < client_pairs[0].topic_size; i++)
                            {
                                if (i == 0)
                                    printf("%s", client_pairs[0].topics[i]);
                                else
                                    printf(", %s", client_pairs[0].topics[i]);
                            }
                            printf("]\n");

                            // Remember to free memory allocated by strdup
                            free(data[0]);
                        }
                    }
                }

                if (send(new_fd, buf, strlen(buf), 0) == -1)
                    perror("send");
            }
        }
        else
        {
            print_err("Fork fail");
        }
        printf("Closing fd %d\n", new_fd);
        close(new_fd); // parent doesn't need this
    }
    printf("Closing sockfd %d\n", sockfd);
    close(sockfd); // child doesn't need the listener

    // release all client pair memory
    // free(client_pairs[0].topics[0]);
    // free(client_pairs[0].topics);
    // free(client_pairs);
    return 0;
}