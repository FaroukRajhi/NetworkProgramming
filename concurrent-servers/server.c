#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>

typedef enum { WAIT_FOR_MSG, IN_MSG} processingState;

void serve_connection(int sockfd)
{
    if(send(sockfd, "*", 1, 0) < 1)
    {
        perror("send error!");
    }

    processingState state = WAIT_FOR_MSG;


    while(1)
    {
        uint8_t buf[1025];
        int len = recv(sockfd, buf, sizeof(buf), 0); /* recv is a syscall is  used to receive messages from a socket*/
        if(len < 0 )
        {
            perror("receive error!");
        }else if (len == 0)
        {
            break;
        }


        for(int i = 0; i < len; ++i)
        {
            switch (state)
            {
              case WAIT_FOR_MSG:
                if(buf[i] == '^')
                {
                    state = IN_MSG;
                }
                break;
              case IN_MSG:
               if(buf[i] == '$')
               {
                 state = WAIT_FOR_MSG;
               } else {
                buf[i] += 1;
                if(send(sockfd, &buf[i], 0, 1) < 1)
                {
                    perror("send error");
                    close(sockfd);

                    return;
                }
               }
               break;
            }

        }

        close(sockfd);

    }
}
int listen_inet_socket(int portnum);

int listen_inet_socket(int port) {

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    int opt = 1;

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
                   &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *)&server_addr,
             sizeof(server_addr)) < 0) {
        perror("bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, 5) < 0) {
        perror("listen");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    return sockfd;
}
int main(int argc, char** argv)
{
    setvbuf(stdout, NULL, _IONBF, 0); /* unbuffered buffer*/

    int port = 8888;

    if(argc >= 2)
    {
        port = atoi(argv[1]);
    }

    printf("Serving on port %d\n", port);

    int sockfd = listen_inet_socket(port);

    
    while(1)
    {
        struct sockaddr_in client_addr;

        socklen_t client_addr_len = sizeof(client_addr);

        int new_socket = accept(
            sockfd,
            (struct sockaddr *)&client_addr,
            &client_addr_len
        );
        if(new_socket < 0)
        {
            perror("accept error!");
        }

        printf("peer done\n");
    }

    return 0;
}