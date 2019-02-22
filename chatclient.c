/***********************************************************
 * Author:          Kelsey Helms
 * Date Created:    May 4, 2017
 * Filename:        chatclient.c
 *
 * Overview:
 * This implements the C client chat
 ************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>


/* ************************************************************************
                Definitions
 ************************************************************************ */

#define MAX_MESSAGE_LENGTH  500


/* ************************************************************************
                Functions
 ************************************************************************ */

void getHandle(char *input);
struct addrinfo *getAddressInfo(char *address, char *port);
int setupConnection(struct addrinfo *res);
void handshake(int sockfd, char *handle, char *servername);
void chat(int sockfd, char *handle, char *servername);


/***********************************************************
 * main: runs the program.
 *
 * parameters: server address and port number.
 * returns: none.
 ***********************************************************/

int main(int argc, char *argv[])
{
	if(argc != 3)	   //if we have too few or too many args, print error and exit
	{
		fprintf(stderr, "Usage: %s <host> <port>\n", argv[0]);
		exit(1);
	}
   
	char handle[10];	//char array for handle
	getHandle(handle);	  //get handle from user

	struct addrinfo *res = getAddressInfo(argv[1], argv[2]);   //get address info
	
	int sockfd = setupConnection(res);	//create and connect socket

	char servername[10];	//char array for server handle
	handshake(sockfd, handle, servername);  //perform handshake

	chat(sockfd, handle, servername);   //chat!

	freeaddrinfo(res);  //free address struct
	
	return 0;
}


/***********************************************************
 * getHandle: gets a handle from user.
 *
 * parameters: char array pointer.
 * returns: none.
 ***********************************************************/

void getHandle(char *input)
{
	printf("Please enter a 10-character usename: ");
	scanf("%s", input);
}


/***********************************************************
 * getAddressInfo: gets address info from user for address 
 * info struct.
 *
 * parameters: address char array pointer and port char 
 * array pointer.
 * returns: address info struct.
 ***********************************************************/

struct addrinfo * getAddressInfo(char *address, char *port)
{
	int status;	 //status indicator
	struct addrinfo hints, *res;	//address info structs for constraints and results

	memset(&hints, 0, sizeof hints);	//clear out address struct
	hints.ai_family = AF_INET;	  //set up for IPv4 addresses
	hints.ai_socktype = SOCK_STREAM;	//set up for TCP

	if((status = getaddrinfo(address, port, &hints, &res)) != 0)	//get internet address for connection
	{
		fprintf(stderr,
				"getaddrinfo error: %s\nDid you enter the correct IP/Port?\n",
				gai_strerror(status));  //if not successful, print error and exit
		exit(1);
	}
	
	return res;	 //return resulting address info struct
}


/***********************************************************
 * setupConnection: creates socket and connects it to 
 * server.
 *
 * parameters: address info struct.
 * returns: socket file descriptor.
 ***********************************************************/

int setupConnection(struct addrinfo *res)
{
	int sockfd;	 //socket file descriptor
	int status;	 //status of socket

	if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1)	//create socket
	{
		fprintf(stderr, "Error creating socket\n");	 //if not created, print error and exit
		exit(1);
	}

	if ((status = connect(sockfd, res->ai_addr, res->ai_addrlen)) == -1)	//connect to server
	{
		fprintf(stderr, "Error connecting socket\n");	   //if not connected, print error and exit
		exit(1);
	}
	
	return sockfd;	  //return socket file descriptor
}



/***********************************************************
 * handshake: sends handles from client to server and vis 
 * versa.
 *
 * parameters: socket file descriptor, handle, and pointer
 * to empty handle char array.
 * returns: none.
 ***********************************************************/

void handshake(int sockfd, char *handle, char *servername)
{
	int sendit = send(sockfd, handle, strlen(handle), 0);	  //send handle to server
	int receiveit = recv(sockfd, servername, 10, 0);	   //receive handle from server
}


/***********************************************************
 * chat: client initiates conversation and alternates chat.
 *
 * parameters: socket file descriptor, handle, and server's
 * handle.
 * returns: none.
 ***********************************************************/

void chat(int sockfd, char *handle, char *servername)
{
	char input[MAX_MESSAGE_LENGTH+3];	//input buffer
	char output[MAX_MESSAGE_LENGTH+1];   //output buffer
	int sentBytes = 0;	  //store length of bytes sent
	int status;	 //status indicator

	memset(input,0,sizeof(input));	 //clear out buffers
	memset(output,0,sizeof(output));
	fgets(input, MAX_MESSAGE_LENGTH, stdin);	//get input from user
	
	while(1)	//stay in chat until "\quit"
	{
		printf("%s> ", handle);	 //print user handle
		fgets(input, MAX_MESSAGE_LENGTH+2, stdin);	//get input from user

		if (strcmp(input, "\\quit\n") == 0)	 //if input is "\quit", then close connection
		{
			break;
		}

		sentBytes = send(sockfd, input, strlen(input) ,0);  //send message to server

		if(sentBytes == -1)	 //if error, print error and quit
		{
				fprintf(stderr, "Error sending data to host\n");
				exit(1);
		}

		status = recv(sockfd, output, MAX_MESSAGE_LENGTH, 0);	  //get message from server

		if (status == -1)	   //if error, print error and quit
		{
			fprintf(stderr, "Error when receiving data from host\n");
			exit(1);
		}
		else if (status == 0)   //server closed connection
		{
			printf("Connection closed by server\n");	//print indicator and close connection
			break;
		}
		else
		{
			printf("%s> %s\n", servername, output);	 //print message
		}
		
		memset(input,0,sizeof(input));	 //clear out buffers
		memset(output,0,sizeof(output));
	}

	close(sockfd);	  //close connection
	printf("Closed Connection\n");	  //print indicator
}
