/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#include <iostream>
#include <string>
#include <vector>
#include <bits/stdc++.h>
#include <sstream>


#define PORT "3490" // the port client will be connecting to 

// colored print outputs
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define END     "\033[0m"

#define MAXDATASIZE 100 // max number of bytes we can get at once 

using namespace std;

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void print_err(const char *msg)
{
    printf("%s%s%s", RED, msg, END);
}

void print_success(const char *msg)
{
    printf("%s%s%s", GREEN, msg, END);
}

void print_help(const char *msg)
{
    printf("%s%s%s", YELLOW, msg, END);
}

string waitForInput() {
    string input;
    cout << "Enter a command: ";
    getline(cin, input); // Wait for user input
    return input;
}

int main(int argc, char *argv[])
{
    int sockfd, numbytes;  
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    string test;
    char s[INET6_ADDRSTRLEN];
    vector<string> stringVector;
    const char *hostname;
    const char *port;

    while(true)
    {
        string userInput = waitForInput();
        stringstream ss(userInput);
        cout << "You entered: " << userInput << endl;
        size_t found = userInput.find("CONNECT");
        if (found == 0)
        {
         while (getline(ss, test, ' ')) {
 
        // store token string in the vector
        stringVector.push_back(test);
    }
    // print the vector
    // for (int i = 0; i < stringVector.size(); i++) {
    //     cout << stringVector[i] << endl;
    // }

        break;
        }
    }
    
    if (stringVector.size() != 3) {
        print_err("Error: ");
        printf( "Invalid cmd arguments\n");
        printf("Expected number of cmd arguments is 2. Received cmd argument number is %d\n", stringVector.size());
        fprintf(stderr,"Usage: CONNECT <port> <hostname>. For example: CONNECT 3490 localhost \n%s",YELLOW, END);
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hostname = stringVector[2].c_str();
    port = stringVector[1].c_str();
    if ((rv = getaddrinfo(hostname, port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure
    while(true)
    {
    string userInput = waitForInput();
     if (send(sockfd, &userInput[0], userInput.size(), 0) == -1)
                perror("send");

    // if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
    //     perror("recv");
    //     exit(1);
    // }

    // buf[numbytes] = '\0';

    // printf("client: received '%s'\n",buf);
    }
    close(sockfd);

    return 0;
}