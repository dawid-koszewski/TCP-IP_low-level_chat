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
#include <sys/wait.h>
#include <signal.h>


void sigchld_handler(int s)
{
    int saved_errno = errno; //waitpid() might overwrite errno, so we save and restore it

    while (waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}


void* get_in_addr(struct sockaddr* sa)
{
    if (sa->sa_family == AF_INET)
        return &((struct sockaddr_in*)sa)->sin_addr;
    return &((struct sockaddr_in6*)sa)->sin6_addr;
}



int main(int argc, char *argv[])
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo *servinfo = NULL;
    int status = -1;
    if ((status = getaddrinfo(NULL, "3490", &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return 1;
    }

    struct addrinfo *p = NULL;
    int socketFD = -1;
    int yes = 1;
    for (p=servinfo; p != NULL; p=p->ai_next)
    {
        if ((socketFD = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1)
        {
            perror("server: socket");
            continue;
        }

        if (setsockopt(socketFD, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        {
            perror("setsockopt");
            exit(1);
        }

        if (bind(socketFD, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(socketFD);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo);
    servinfo = NULL; //I added that

    if (p == NULL)
    {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }
    p = NULL; //I added that

    if (listen(socketFD, 10) == -1)
    {
        perror("listen");
        exit(1);
    }

//I HAVE NO IDEA WHAT THESE LINES ARE RESPONSIBLE FOR:
    struct sigaction sa;
    sa.sa_handler = sigchld_handler; //reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(1);
    }
//============


    while(1)
    {
        printf("Waiting for client to connect...\n");

        struct sockaddr_storage their_addr;
        socklen_t addr_size = sizeof their_addr;

        int newFD = accept(socketFD, (struct sockaddr*)&their_addr, &addr_size);
        if (newFD == -1)
        {
            perror("accept");
            exit(1);
        }

        char client_name[INET6_ADDRSTRLEN];
        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr*)&their_addr), client_name, sizeof client_name);

        printf("server: got connection from client %s\n", client_name);

    //-------------------------------------------------
//
//        if (!fork()) //this is child process
//        {
//            close(socketFD); //child doesn't need the listener
//
//            if (send(newFD, "Hello World!", 13, 0) == -1)
//                perror("send");
//
//            close(newFD);
//            exit(0);
//        }
//
//        close(newFD); //parent doesn't need this
//    }


    //--------------------------------
        char buffsend[128];
        char buffrecv[128];

        while(true)
        {
        //SENDING
            printf("Enter your message for client: ");
            fgets(buffsend, 128-1, stdin);

            int bytes_sent = send(newFD, buffsend, strlen(buffsend), 0);
            if (bytes_sent <= 0)
            {
                perror("send");
                close(newFD);
                close(socketFD);
                exit(1);
            }

        //RECEIVING
            printf("\n...waiting for message from client...\n");
            int bytes_rcvd = recv(newFD, buffrecv, 128-1, 0);
            if (bytes_rcvd <= 0)
            {
                perror("recv");
                close(newFD);
                close(socketFD);
                exit(1);
            }

            buffrecv[bytes_rcvd] = '\0';
            printf("[Message from client]: %s\n", buffrecv);
        }
        close(newFD);
    //-----------------------------------------------------
    }

    close(socketFD); //to fully free FD

    return 0;
}

