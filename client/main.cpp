#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h> //to get close() function to work



int main()
{
    struct addrinfo hints_c, *res_c;
    int status_c;

    memset(&hints_c, 0, sizeof hints_c);
    hints_c.ai_family = AF_UNSPEC;
    hints_c.ai_socktype = SOCK_STREAM;

    if ( (status_c = getaddrinfo("dawid-VirtualBox", "3490", &hints_c, &res_c)) != 0 )
    {
        printf("getaddrinfo error: %s\n", gai_strerror(status_c));
        return 2;
    }

    int socketFD_c = socket(res_c->ai_family, res_c->ai_socktype, res_c->ai_protocol);
    bind(socketFD_c, res_c->ai_addr, res_c->ai_addrlen);

    connect(socketFD_c, res_c->ai_addr, res_c->ai_addrlen);
//-------------------------------------------------

    char buffsend[128];
    char buffrecv[128];

    while(true)
    {
    //RECEIVING
        printf("\n...waiting for message from server...\n");
        int bytes_rcvd = recv(socketFD_c, buffrecv, 128-1, 0);
        if (bytes_rcvd <= 0)
        {
            printf("Either connection closed or error\n");
            break;
        }
        buffrecv[bytes_rcvd] = '\0';
        printf("[Message from server]: %s\n", buffrecv);

    //SENDING
        printf("Enter your message for server: ");
        fgets(buffsend, 128-1, stdin);

        int bytes_sent = send(socketFD_c, buffsend, strlen(buffsend), 0);
        if (bytes_sent == 0)
        {
            printf("Failure sending message\n");
            close(socketFD_c);
            break;
        }
    }


    close(socketFD_c); //to fully free FD
    freeaddrinfo(res_c);

    return 0;
}

