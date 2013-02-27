/*
 *  Lab Assignment 2 - CS 558L - Internetwork and distributed lab
 *  Name:   Aniket Zamwar
 *  Email:  zamwar@usc.edu
 *  Date:   6th Sept, 2012
 *  Module: Server Socket Program
 */

/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>

void error(const char *msg){
    perror(msg);
    exit(1);
}

/*
 * Global Variables
 */
int sockfd, flag = 0;

/*
 * Signal Handler
 */

void INThandler(int sig)
{
  char  c;

  signal(sig, SIG_IGN);
  printf("OUCH, did you hit Ctrl-C?\n"
	 "Do you really want to quit? [y/n] ");
  c = getchar();
  if (c == 'y' || c == 'Y'){
    flag = 1;
    close(sockfd);
  }
  else
    signal(SIGINT, INThandler);
}

int main(int argc, char *argv[])
{
     int portno, newsockfd;
     socklen_t clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n, pid, optval;
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     
     sockfd = socket(AF_INET, SOCK_DGRAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     optval = 1;
     setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&optval, sizeof optval);
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
       error("ERROR on binding");

     // signal(SIGINT, INThandler);
     clilen = sizeof(cli_addr);

     while(1 && flag == 0) {
       printf("Waiting to receive message.\n");
       bzero((char *) buffer, sizeof(buffer));
       
       n = recvfrom(sockfd,buffer,1024,0,(struct sockaddr *)&cli_addr,&clilen);
       if(flag == 1) {
           printf("Control-C Pressed, Exiting Program.\nThank you for using it.\n");
           break;
       }
       if (n < 0)
           error("Error recvfrom");
       printf("From %s> Received Message: %s\n",inet_ntoa(cli_addr.sin_addr),buffer);
       n = sendto(sockfd,"Got your message ",17, 0,(struct sockaddr *) &cli_addr,clilen);
       if (n  < 0) 
           error("Error sendto");
     } /* end while */
     
     close(sockfd);
     return 0; 
}


