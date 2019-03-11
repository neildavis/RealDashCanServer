/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
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
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    int n, count;
    
    if (argc < 2) {
        fprintf(stderr,"WARNING, no port provided, using default port: %d\n", DEFAULT_PORT_NO);
        portno = DEFAULT_PORT_NO;
    }
    else {
        portno = atoi(argv[1]);
    }
    signal(SIGPIPE, SIG_IGN);   // Don't crash on SIGPIPE
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("ERROR opening socket");
    }
    // setsockopt: Handy debugging trick that lets us rerun the server immediately after we kill it;
    //   // otherwise we have to wait about 20 secs. Eliminates "ERROR on binding: Address already in use" error.
    //
    int optval = 1; /* flag value for setsockopt */
    setsockopt( sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR on binding");
    }
    listen(sockfd,5);
    while (1) {
        printf("Listening for incoming RealDash CAN connections...\n");
        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) {
            printf("ERROR on accept\n");
            break;
        }
        printf("Accepted conection from %s\n", inet_ntoa(cli_addr.sin_addr));
        count = 1;
        
        // Consts for now
        uint16_t rpm = 3500;
        uint16_t speedMph = 120;
        uint16_t fuelPercent = 25;
        char gear = 0;
        
        while (1) {
            printf("Write CAN data %d\n", count);
            // Write sync marker and frame id to socket
            n = write(newsockfd,"\x44\x33\x22\x11"  // sync marker
                      "\x80\x0c\x00\x00"  // frame id (3200)
                      ,8);                // num bytes to write
            if (n < 0) {
                printf("ERROR writing sync marker and frame id to socket\n");
                break;
            }
            // Build CAN frame data
            char canFrame[7]= {0};
            // RPM
            canFrame[0] = (rpm & 0xff00) >> 8;
            canFrame[1] = rpm & 0x00ff;
            // Speed - MPH
            canFrame[2] = (speedMph & 0xff00) >> 8;
            canFrame[3] = speedMph & 0x00ff;
            // Fuel - %
            canFrame[4] = (fuelPercent & 0xff00) >> 8;
            canFrame[5] = fuelPercent & 0x00ff;
            canFrame[6] = gear;
            // Write CAN frame to socket
            n = write(newsockfd,canFrame,sizeof(canFrame));
            if (n < 0) {
                printf("ERROR writing CAN frame to socket\n");
                break;
            }
            fflush( stdout );
            sleep_ms(500);
            ++count;
        }
        printf("Connection from %s closed\n", inet_ntoa(cli_addr.sin_addr));
        fflush( stdout );
        close(newsockfd);
    }
    printf("Listening socket closed\n");
    fflush( stdout );
    close(sockfd);
    return 0;
}
