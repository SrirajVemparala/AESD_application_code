/*
Name: project-socket.c
Description:  socket based client program
Author: Isha Sharma
Reference: 
	https://beej.us/guide/bgnet/html/#what-is-a-socket
	https://github.com/thlorenz/beejs-guide-to-network-samples
	https://www.gnu.org/software/libc/manual/html_node/Termination-in-Handler.html
    http://cslibrary.stanford.edu/103/LinkedListBasics.pdf

Course: AESD
Date: April 15, 2024
*/

// Includes
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/stat.h> 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <syslog.h>
#include <fcntl.h>
#include <stdbool.h>

#include "wiringPi.h"
#include "lcd.h"

//Defines 
#define PORT        9000
#define BACKLOG     (10) 
#define BUFFER_LEN  (1024)

#define LCD_RS  25               //Register select pin
#define LCD_E   24               //Enable Pin

#define LCD_D4  23               //Data pin D4
#define LCD_D5  22               //Data pin D5
#define LCD_D6  21               //Data pin D6
#define LCD_D7  14               //Data pin D7

//Variables
int socket_fd;	//for client 
volatile sig_atomic_t fatal_error_in_progress = 0;
pid_t pid;
bool daemon_flag = false;

struct sockaddr_in server_address; // connector's address information

char s[INET6_ADDRSTRLEN];

int status;
float buffer [BUFFER_LEN];

//smooth cleaup and termination 
void cleanup(void)
{	
    syslog(LOG_INFO, "cleaning up \n");	

	close(socket_fd);
	shutdown(socket_fd, SHUT_RDWR);

	exit(EXIT_SUCCESS);
	
}

//signal handler for sigint/sigterm

//signal handler for sigint/sigterm
void signal_handler(int signal_type)
{
	if(signal_type == SIGINT || signal_type == SIGTERM)
	{
		syslog(LOG_INFO,"Caught signal, exiting\n");
		
		//The cleanest way for a handler to terminate the process is to raise the same signal that ran the handler in the first place. 
		/* Since this handler is established for more than one kind of signal, 
		     it might still get invoked recursively by delivery of some other kind
		     of signal.  Use a static variable to keep track of that. 
		*/
		     
		  if (fatal_error_in_progress)
		    raise (signal_type);
		  fatal_error_in_progress = 1;

		  /*STEP:
		  	Gracefully exits when SIGINT or SIGTERM is received
		 */
			shutdown(socket_fd, SHUT_RDWR);
            close(socket_fd);
            syslog(LOG_INFO,"cleaned up\n");

		  /* Now reraise the signal.  We reactivate the signal’s
		     default handling, which is to terminate the process.
		     We could just call exit or abort,
		     but reraising the signal sets the return status
		     from the process correctly. 
		  */
		  signal (signal_type, SIG_DFL);
		  raise (signal_type);
	}
}


//function to make deamon
void make_deamon(void)
{
     pid = fork();
    		
    		if(pid <0)
    		{
    			syslog(LOG_ERR,"deamon fork failed \n");
    			exit (EXIT_FAILURE);
    		}
    		else if (pid>0)
    		{
    			// this is the parent, terminate this
    			syslog(LOG_INFO,"Parent terminated\n");
			    exit(0);
    		}
    		    		
    		umask(0);
    		//create a new session
    		pid_t sid = setsid();
    		if( sid <0)
    		{
    			syslog(LOG_ERR,"setsid failed");
    			exit(EXIT_FAILURE);
    		}
    		
    		// change dir to root
    		if (chdir("/") < 0)
            	{

                perror("changing directory to root failed\n");
                exit(-1);
            	}
    		
    		//close fds
		close(STDIN_FILENO);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);
		
		//redirect to dev null
		open("/dev/null", O_RDWR);
}

//function to open a stream socket bound to port 9000
int open_socket()
{

    if ((socket_fd = socket(PF_INET, SOCK_STREAM, 0)) == -1) 
    {
        syslog(LOG_ERR,"Socket creation failed \n");
        return -1;
    }
    syslog(LOG_INFO, "socket creation sucess \n");
    printf("socket sucess \n");

    return 0;
}

//entry point
int main(int argc, char *argv[])
{
    const char *server_ipaddr = "192.168.170.2";
    int lcd;     


	//for syslogging throughout
    openlog("Client Socket:",0,LOG_USER);
    syslog(LOG_INFO,"starting socket\n");

	if (argc == 2)
	{
		if(strcmp(argv[1], "-d") == 0) 
		{
			// daemon mentioned 
			// initialise daemon after getaddrinfo
			daemon_flag = true;
			syslog(LOG_INFO,"Deamon mentioned\n");
		}
	}
	
	//register signals SIGINT and SIGTERM
	if (signal (SIGINT, signal_handler) == SIG_ERR) 
	{
		syslog(LOG_ERR,"SIGINT handler failed \n");
		exit (EXIT_FAILURE);
	}
	if (signal (SIGTERM, signal_handler) == SIG_ERR) 
	{
		syslog(LOG_ERR,"SIGINT handler failed \n");
		exit (EXIT_FAILURE);
	}
    
    //init wiring pi and lcd   
    wiringPiSetup();        
    lcd = lcdInit (2, 16, 4, LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7, 0, 0, 0, 0);
    
    //display initial messages
    lcdPosition(lcd, 2, 1); 
    lcdPuts(lcd, "AESD PROJECT");    
    delay(1000);
    lcdClear(lcd);
    delay(1000);
    lcdPosition(lcd, 2, 0);
    lcdPuts(lcd, "TEMP SYSTEM"); 
    
    /* STEP:
        open socket
    */	
    if (open_socket() == -1)
    {
        syslog(LOG_ERR, "open socket failed \n");
    }
                
    /* STEP:
        if deamon specified, initialise it
        fork->create a new sid->change directory to root-> close and redirect stdin/err/out to /dev/null
    */
    if(daemon_flag)
    {
        make_deamon();
    }
                
    /* STEP:
        connect with server
    */
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = PF_INET;
    server_address.sin_addr.s_addr = inet_addr(server_ipaddr);
    server_address.sin_port = htons(PORT); 

    status = connect(socket_fd, (struct sockaddr *)&server_address, sizeof(server_address));
    if (status == -1)
    {
        syslog(LOG_ERR, "connection failed \n");
        close(socket_fd);
        exit(-1);
    }
    printf("Connected to %s \n", server_ipaddr);
    
    /* STEP:
        receive temp values from socket server and print on lcd
    */
    while(1)
    {
    	//store in a temp buffer
        int bytes_rec = recv(socket_fd, buffer, 1024, 0);
	
	//print values from the buffer 
        if (bytes_rec == 0)
        {
        	printf("no data\n");
        }
	else if(bytes_rec > 0)
        {
            lcdClear(lcd); 
            lcdPosition(lcd, 0, 0); 
	    for (int i = 0; i < (int)(bytes_rec / sizeof(float)); i++)
            {
		    printf("%.2f\n", buffer[i]);   
		    lcdPrintf(lcd, "Temp: %.2f degC", buffer[i]);
            }
        }
	}   
           
    cleanup(); //cleanup after end
    return 0;
}

