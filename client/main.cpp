#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //to get close() function to work
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>


void* get_in_addr(struct sockaddr* sa)
{
    if (sa->sa_family == AF_INET)
        return &((struct sockaddr_in*)sa)->sin_addr;
    return &((struct sockaddr_in6*)sa)->sin6_addr;
}



int main(int argc, char *argv[])
{
//    if (argc != 2) //and first parameter in getaddrinfo(argv[1], ...)
//    {
//        fprintf(stderr, "usage: client hostname\n");
//        exit(1);
//    }

    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo *servinfo = NULL;
    int status = -1;
    if ( (status = getaddrinfo("dawid-VirtualBox", "3490", &hints, &servinfo)) != 0 )
    {
        printf("getaddrinfo error: %s\n", gai_strerror(status));
        return 1;
    }

    struct addrinfo *p = NULL;
    int socketFD = -1;
    for (p=servinfo; p != NULL; p=p->ai_next)
    {
        if ((socketFD = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1)
        {
            perror("client: socket");
            continue;
        }

        if (connect(socketFD, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(socketFD);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL)
    {
        fprintf(stderr, "client: failed to connect\n");
        exit(1);
    }

    char server_name[INET6_ADDRSTRLEN];
    inet_ntop(p->ai_family, get_in_addr((struct sockaddr*)p->ai_addr), server_name, sizeof server_name);

    printf("client: connecting to server %s\n", server_name);

    p = NULL; //I added that
    freeaddrinfo(servinfo);
    servinfo = NULL; //I added that



//-------------------------------------------------

    char buffsend[128];
    char buffrecv[128];

    while(true)
    {
    //RECEIVING
        printf("\n...waiting for message from server...\n");
        int bytes_rcvd = recv(socketFD, buffrecv, 128-1, 0);
        if (bytes_rcvd <= 0)
        {
            perror("recv");
            close(newFD);
            close(socketFD);
            exit(1);
        }
        buffrecv[bytes_rcvd] = '\0';
        printf("[Message from server]: %s\n", buffrecv);

    //SENDING
        printf("Enter your message for server: ");
        fgets(buffsend, 128-1, stdin);

        int bytes_sent = send(socketFD, buffsend, strlen(buffsend), 0);
        if (bytes_sent <= 0)
        {
            perror("send");
            close(newFD);
            close(socketFD);
            exit(1);
        }
    }


    close(socketFD); //to fully free FD

    return 0;
}

