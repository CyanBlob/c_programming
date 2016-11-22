/* 
 * A simple server in the internet domain using TCP
 * The port number is passed as an argument
 * http://www.linuxhowtos.org/C_C++/socket.htm
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[])
{
	int sockfd, newsockfd, portno;
	socklen_t clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	int n;
	int pid;
	static int numClients = 0;

	if (argc < 2) 
	{
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);
	}

	// server file descriptor
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) 
	{
		error("ERROR opening socket");
	}

	// zero-out server sockaddr_in
	bzero((char *) &serv_addr, sizeof(serv_addr));

	// convert port from string to int, storing in portno
	portno = atoi(argv[1]);

	// set server options
	serv_addr.sin_family = AF_INET;		// internet mode, not UNIX
	serv_addr.sin_addr.s_addr = INADDR_ANY;	// use current IP
	serv_addr.sin_port = htons(portno);	// convert portno to valid format

	// bind to the port
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
		error("ERROR on binding");
	}

	// begin listening on the port
	listen(sockfd,5);

	clilen = sizeof(cli_addr);

	// main infinite-loop
	// Receive new fd using accept, then fork
	while (1)
	{
		// client file descriptor
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

		// fork, closing socket if fork failed
		if ( (pid=fork()) == -1)
		{
			close(newsockfd);
			continue;
		}

		// parent
		if (pid > 0)
		{
			close(newsockfd);
			numClients++;
			printf("%d clients have connected\n", numClients);
			continue;
		}

		// child
		else if (pid == 0)
		{
			while (1)
			{
				numClients++;
				if (newsockfd < 0) 
				{
					printf("ERROR on accept\n");
					numClients--;
					return 1;
				}

				bzero(buffer,256);

				n = read(newsockfd,buffer,255);

				if (n < 0)
				{
					printf("ERROR reading from socket\n");
					numClients--;
					return 1;
				}

				printf("Message from %d: %s\n", newsockfd, buffer);

				n = write(newsockfd,"I got your message",18);

				if (n < 0)
				{
					printf("ERROR writing to socket\n");
					numClients--;
					return 1;
				}
			}
		}
	}

	close(newsockfd);
	close(sockfd);

	return 0; 
}
