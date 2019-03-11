/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#if _POSIX_C_SOURCE >= 199309L
#include <time.h>   // for nanosleep
#else
#include <unistd.h> // for usleep
#endif

const int DEFAULT_PORT_NO = 35000;

void sleep_ms(int milliseconds) // cross-platform sleep function
{
#if _POSIX_C_SOURCE >= 199309L
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
#else
    usleep(milliseconds * 1000);
#endif
}

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno, clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n, count;
     if (argc < 2) {
         fprintf(stderr,"WARNING, no port provided, using default port: %d\n", DEFAULT_PORT_NO);
         portno = DEFAULT_PORT_NO;
     }
     else {
         portno = atoi(argv[1]);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     // setsockopt: Handy debugging trick that lets us rerun the server immediately after we kill it; 
     //   // otherwise we have to wait about 20 secs. Eliminates "ERROR on binding: Address already in use" error. 
     //
     int optval = 1; /* flag value for setsockopt */
     setsockopt( sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));
     bzero((char *) &serv_addr, sizeof(serv_addr));
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     listen(sockfd,5);
     printf("Listening...\n");
     fflush( stdout );
     clilen = sizeof(cli_addr);
     newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
     if (newsockfd < 0) 
          error("ERROR on accept");
     printf("Accepted!\n");
     fflush( stdout );
     bzero(buffer,256);
     count = 1;
     while (1) {
	 printf("Write CAN data %d\n", count);
         fflush( stdout );
         n = write(newsockfd,"\x44\x33\x22\x11\x80\x0c\x00\x00\x04\xb0",10);
         if (n < 0) error("ERROR writing to socket");
	 sleep_ms(500);
	 ++count;
     }
     return 0; 
}
