#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    int  send_buffer_size = 2560;
    char send_buffer[send_buffer_size];
    int  read_buffer_size = 2560;
    char read_buffer[read_buffer_size];

    if (argc < 3)
    {
        printf("usage: ./a.out (host) (path)\n");
        exit(0);
    }
    printf("host = %s\n", argv[1]);
    printf("path = %s\n", argv[2]);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(80);

    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");


    sprintf(send_buffer, "GET %s HTTP/1.1\r\nHost: %s\r\nUser-Agent: curl/7.58.0\r\nAccept: */*\r\n\r\n", argv[2], argv[1]);
    n = write(sockfd,send_buffer,strlen(send_buffer));
    if (n < 0) 
         error("ERROR writing to socket");

    bzero(read_buffer, read_buffer_size);
    n = read(sockfd,read_buffer,read_buffer_size);
    if (n < 0) 
         error("ERROR reading from socket");
    printf("%s\n",read_buffer);
    close(sockfd);
    return 0;
}
