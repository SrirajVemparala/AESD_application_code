/*******************************************************************************
 * aesdsocket.c
 * Date:        22-04-2024
 * Author:      Raghu Sai Phani Sriraj Vemparala, raghu.vemparala@colorado.edu
 * Description: This file has data related to server.c
 * References: Beige Guide
 *
 *
 ******************************************************************************/
#include<stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h> 
#include <syslog.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include "../MLX_code/mlxtest.h"

/***********GLOBAL_VARIBLES*************/
struct addrinfo hints;
struct addrinfo* servinfo;
struct sockaddr* socketaddr;
int sock_fd;
bool sig_trig = false;
/*
static void signal_handler(int signo)
{
    // printf("In_sig_hand\n");
    // printf("%d\n",signo);
    // if(signo == SIGINT)
    // {
    //     syslog(LOG_INFO,"SIGINT detected");
    // }
    // else if(signo == SIGTERM)
    // {
    //     syslog(LOG_INFO,"SIGTERM detected");
    // }
    sig_trig = true;//Trigger signal handling for cleanup
    //printf("sigtrig:%d\n",sig_trig);
    //exit(EXIT_SUCCESS);
}*/


/**********DEFINES**********/
#define REC_LEN 128
#define FILE_DATA_LEN 1024


void thread_rec_data(int *sock_accept_fd)
{
    int accepted = 1,data_sent;   
    while (accepted)
    {   
            /* code */
            float temp = get_temp_data();
            
            data_sent = send(*sock_accept_fd, &temp, sizeof(float), 0);
            if(data_sent == -1)
            {
                syslog(LOG_ERR,"Error in sending the data");
                printf("Send data failed\n");
                printf("file_read_error\n");
                close(*sock_accept_fd);                
                    return;
            }


                //Close the socket
		        //syslog(LOG_ERR, "Closed connection with %s\n", inet_ntoa(((struct sockaddr_in*)&their_addr)->sin_addr));
            
    }
    close(*sock_accept_fd);
}
 
int main(int argc, char* argv[])
{

    int daemon_flag = 0;
    //Port ID
    const char* service = "9000";
    //Logging start
    openlog(NULL,LOG_PID, LOG_USER);

    //Initialize head to NULL

    if((argc>1) && strcmp(argv[1],"-d")==0)//Deamon mode entry
    {
       
        daemon_flag = 1;
    }
/*
 * Register signal_handler as our signal handler
 * for SIGINT.
 */
 /*if (signal (SIGINT, signal_handler) == SIG_ERR) {
 fprintf (stderr, "Cannot handle SIGINT!\n");
 syslog(LOG_ERR, "Cannot handle SIGINT");
 exit (EXIT_FAILURE);
 }*/
 /*
 * Register signal_handler as our signal handler
 * for SIGTERM.
 */
 /*if (signal (SIGTERM, signal_handler) == SIG_ERR) {
 fprintf (stderr, "Cannot handle SIGTERM!\n");
  syslog(LOG_ERR, "Cannot handle SIGTERM");
 exit (EXIT_FAILURE);
 }*/
    i2c_init();
    //length of socket
    socklen_t soclen = sizeof(struct sockaddr);
    //Creation of socket
    sock_fd = socket(PF_INET,SOCK_STREAM,0);
    memset(&hints,0,sizeof(struct addrinfo));
    if(sock_fd == -1)
    {
        perror("Socketfd failed");
        printf("Socketfd failed\n");
        syslog(LOG_ERR, "Scoket Creation failed");
        exit(EXIT_FAILURE);
    }
    //int flags = fcntl(sock_fd, F_GETFL, 0);
    //fcntl(sock_fd, F_SETFL, flags | O_NONBLOCK);
    int sock_accept_fd = 0;
    hints.ai_family = AF_INET;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;
    int value = 1;
    //Accept
    struct sockaddr_storage their_addr;
    socklen_t addr_size = 0;
    // Set socket options for reusing address and port
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &value, sizeof(int)))
    {
      printf("Failed to set socket options:%s\n", strerror(errno));
      syslog(LOG_ERR, "Failed to set socket options:%s\n", strerror(errno));
      return -1;
    }
    //Set the socket to an IP address
    int status = getaddrinfo(NULL,service,&hints,&servinfo);
    if(status != 0)
    {
        freeaddrinfo(servinfo);
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }
    socketaddr = servinfo->ai_addr;
    //Bind socket
    int bind_status = bind(sock_fd, socketaddr, soclen);
    if(bind_status != 0)
    {
        freeaddrinfo(servinfo);
        syslog(LOG_ERR, "Bind Socket Failed");
        perror("Bind_error\n");
        printf("Bind_error\n");
        exit(1);
    }
    //Free servinfo
    freeaddrinfo(servinfo);
   
    if(daemon_flag == 1)
    {
         pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if (pid > 0) {
            // Parent process - exit
            exit(EXIT_SUCCESS);
        }

        // Create a new session
        if (setsid() < 0) {
            perror("setsid");
            exit(EXIT_FAILURE);
        }

        // Fork second time
        pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if (pid > 0) {
            // Parent process (first child) - exit
            exit(EXIT_SUCCESS);
        }

        // Change working directory to root to avoid keeping
        // any directory in use that could prevent unmounting
        if (chdir("/") < 0) {
            perror("chdir");
            exit(EXIT_FAILURE);
        }

        // Redirect standard I/O to /dev/null
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
        open("/dev/null", O_RDONLY);
        open("/dev/null", O_WRONLY);
        open("/dev/null", O_RDWR);

    }
    //Listen to messages
    int listen_status = listen(sock_fd, 10);
    if(listen_status)
    {
        perror("Listen Failed\n");
        syslog(LOG_ERR, "Listen Failed");
        //printf("Listen Failed\n");
        exit(EXIT_FAILURE);
    }

    //Rec messages
    addr_size = sizeof(their_addr);
    //memset(rec_val,0,REC_LEN);
    
    //
    //Start of accepting the connections
    //
    // 
   
        //malloc performed
        //printf("Before_accept\n");
        sock_accept_fd = accept(sock_fd, (struct sockaddr *)&their_addr, &addr_size);

        if(sock_accept_fd == -1)
        {
            //perror("Accept Failed");
            //syslog(LOG_ERR, "Unable to accept socket");
            //exit(0);
            printf("accpet_failed");
           
        }
        else
        {
           thread_rec_data(&sock_accept_fd);           
        }
    printf("Iamout\n"); 
    //SIGHANDLING_EXIT_SEQ
    close(sock_fd);   
    close(sock_accept_fd);
    shutdown(sock_fd, SHUT_RDWR);
    closelog();

    exit(EXIT_SUCCESS);

}
