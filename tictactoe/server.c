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
#include <signal.h>
#include <sys/mman.h>
#include <pthread.h>

int sockfd, newsockfd, portno;
static int *globalNumClients;

pthread_t tid[2];

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

void* doSomething(void *arg)
{
	unsigned long i = 0;
	pthread_t id = pthread_self();

	if(pthread_equal(id,tid[0]))
	{
		printf("\n First thread processing\n");
	}
	else
	{
		printf("\n Second thread processing\n");
	}

	printf("arg[0]: %d\n", (int*) arg);

	for(i=0; i<(0xFFFFFFFF);i++);

	return NULL;
}

void interrupt_handler(int sig)
{
	close(sockfd);
	munmap(globalNumClients, sizeof(int));
	exit(0);
}

int main(int argc, char *argv[])
{
	socklen_t clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	int n;
	int pid;
	static int numClients = 0;

	globalNumClients = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
			MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	signal(SIGINT, interrupt_handler);

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

	*globalNumClients = 0;

	// main infinite-loop
	// Receive new fd using accept, then fork
	while (1)
	{

		int i = 0;
		int err;

		// client file descriptor
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

		while (i < 2)
		{
			err = pthread_create(&tid[i], NULL, &doSomething, newsockfd);

			if (err != 0)
				printf("\ncan't create thread :[%s]", strerror(err));
			else
				printf("\n Thread created successfully\n");

			i++;
		}


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
			*globalNumClients = *globalNumClients + 1;
			printf("%d clients have connected\n", *globalNumClients);
			continue;
		}

		// child
		else if (pid == 0)
		{
			while (1)
			{
				if (newsockfd < 0) 
				{
					*globalNumClients = *globalNumClients - 1;
					printf("ERROR on accept\n");
					printf("A client disconnected. There are %d client(s) connected\n", *globalNumClients);
					return 1;
				}

				bzero(buffer,256);

				n = read(newsockfd,buffer,255);

				if (n <= 0)
				{
					*globalNumClients = *globalNumClients - 1;
					printf("ERROR reading from socket\n");
					printf("A client disconnected. There are %d client(s) connected\n", *globalNumClients);
					return 1;
				}

				printf("Message from %d: %s\n", newsockfd, buffer);

				n = write(newsockfd,"I got your message",18);

				if (n < 0)
				{
					*globalNumClients = *globalNumClients - 1;
					printf("ERROR writing to socket\n");
					printf("A client disconnected. There are %d client(s) connected\n", *globalNumClients);
					return 1;
				}
			}
		}
	}

	close(newsockfd);
	close(sockfd);
	munmap(globalNumClients, sizeof(int));

	return 0; 
}
