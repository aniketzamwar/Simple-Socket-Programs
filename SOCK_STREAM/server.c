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
void *SigCatcher(int n) {
  wait3(NULL,WNOHANG,NULL); 
} 

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

/*
 * forked child
 */
void dostuff(int newsockfd) {
  
  char buffer[256];
  int n;
  
  bzero(buffer,256);
  n = read(newsockfd,buffer,255);
  if (n < 0)
    error("ERROR reading from socket");
  printf("Received message: %s\n",buffer);
  n = write(newsockfd,"I got your message",18);
  if (n < 0) 
    error("ERROR writing to socket");
  close(newsockfd);

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
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
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
     if (listen(sockfd,5) < 0)
       error("ERROR on listening");
     signal(SIGCHLD,(void *)SigCatcher);
     signal(SIGINT, INThandler);
     clilen = sizeof(cli_addr);

     while(1 && flag == 0) {
       printf("Waiting to accept connection\n");

       newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
       if(flag == 1){
	 printf("Control-C Pressed, Exiting Program.\nThank you for using it.\n");
	 break;
       }
       printf("Connection Accepted from %s\n",inet_ntoa(cli_addr.sin_addr));
       if (newsockfd < 0){
	 if (EINTR==errno) {
	   continue; /* Restart accept */
	 }
	 error("ERROR on accept");
       }
       pid = fork();
       if (pid < 0) 
	 error("ERROR on fork"); 
       if (pid == 0) { 
	 close(sockfd);
	 dostuff(newsockfd);
	 exit(0); 
       } 
       else 
	 close(newsockfd); 
     } /* end while */
     
     close(sockfd);
     return 0; 
}


