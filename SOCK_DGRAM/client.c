/*                                                                              
 *  Lab Assignment 2 - CS 558L - Internetwork and distributed lab               
 *  Name:   Aniket Zamwar
 *  Email:  zamwar@usc.edu
 *  Date:   6th Sept, 2012
 *  Module: Client Socket Program 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <strings.h>

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
  int sockfd, portno, n,servlen;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
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
    serv_addr.sin_port = htons(portno);
    servlen = sizeof(serv_addr);

    printf("Please enter the message: ");
    bzero(buffer,256);
    fgets(buffer,255,stdin);
    n = sendto(sockfd,buffer,strlen(buffer),0,(struct sockaddr *) &serv_addr,servlen);
    if (n < 0) 
         error("ERROR sendto");
    bzero(buffer,256);
    n = recvfrom(sockfd,buffer,255,0,(struct sockaddr *) &serv_addr,&servlen);
    if (n < 0) 
         error("ERROR reading from socket");
    printf("%s\n",buffer);
    
    close(sockfd);
    return 0;
}
