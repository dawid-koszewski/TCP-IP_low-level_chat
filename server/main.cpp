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
    struct addrinfo hints_s, *res_s;
    int status_s;

    memset(&hints_s, 0, sizeof hints_s);
    hints_s.ai_family = AF_UNSPEC;
    hints_s.ai_socktype = SOCK_STREAM;
    hints_s.ai_flags = AI_PASSIVE;

    if ( (status_s = getaddrinfo(NULL, "3490", &hints_s, &res_s)) != 0 )
    {
        printf("getaddrinfo error: %s\n", gai_strerror(status_s));
        return 2;
    }

    int socketFD_s = socket(res_s->ai_family, res_s->ai_socktype, res_s->ai_protocol);
    bind(socketFD_s, res_s->ai_addr, res_s->ai_addrlen);

    listen(socketFD_s, 10);


    while(1)
    {
        printf("Waiting for next client to connect\n");

        struct sockaddr_storage their_addr;
        socklen_t addr_size = sizeof their_addr;

        int newFD = accept(socketFD_s, (struct sockaddr*)&their_addr, &addr_size);
        if (newFD == -1)
        {
            perror("accept");
            break; //in original code here was: exit;
        }
        printf("Server got connection from client %s\n", inet_ntoa(((struct sockaddr_in*)&their_addr)->sin_addr)); //raw cast to IPv4 !!! will cause errors
    //-------------------------------------------------

        char buffsend[128];
        char buffrecv[128];

        while(true)
        {
        //SENDING
            printf("Enter your message for client: ");
            fgets(buffsend, 128-1, stdin);

            int bytes_sent = send(newFD, buffsend, strlen(buffsend), 0);
            if (bytes_sent == 0)
            {
                printf("Failure sending message\n");
                close(newFD);
                break;
            }

        //RECEIVING
            printf("\n...waiting for message from client...\n");
            int bytes_rcvd = recv(newFD, buffrecv, 128-1, 0);
            if (bytes_rcvd <= 0)
            {
                printf("Either connection closed or error\n");
                break;
            }

            buffrecv[bytes_rcvd] = '\0';
            printf("[Message from client]: %s\n", buffrecv);
        }

        close(newFD);
    }


    close(socketFD_s); //to fully free FD
    freeaddrinfo(res_s);

    return 0;
}

