#include <errno.h>
#include <netinet/ip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BACKLOG 128

int open_port(int port)
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (!sockfd)
	{
		fprintf(stderr, "error creating socket, %s\n",
				strerror(errno));
		return -1;
	}

	struct sockaddr_in addr;
	bzero((char *) &addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);

	if (bind(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0)
	{
		fprintf(stderr, "error binding socket, %s\n", strerror(errno));
		return -1;
	}
	return sockfd;
}

static
int accept_connexion(int sockfd)
{
	struct sockaddr_in addr;
	socklen_t sz = sizeof(addr);
	int clientfd = accept(sockfd, (struct sockaddr *)&addr, &sz);

	if (clientfd < 0)
	{
		fprintf(stderr, "error accepting client, %s\n",
					strerror(errno));
		return -1;
	}
	return clientfd;
}

static
void process_client(int fd)
{
}

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s <port>", argv[0]);
		return -1;
	}

	int port = atoi(argv[2]);
	if (port < 1 || port > 65535)
	{
		fprintf(stderr, "invalid port number %d", port);
		return -1;
	}

	int sockfd = open_port(port);
	if (sockfd < 0)
		return -1;

	listen(sockfd, BACKLOG);

	for (;;)
	{
		int clientfd = accept_connexion(sockfd);
		if (clientfd < 0)
			return -1;

		int pid = fork();
		if (pid < 0)
		{
			fprintf(stderr, "error forking client worker process %s\n",
					strerror(errno));
		}
		else if (pid)
		{
			close(clientfd);
		}
		else
		{
			close(sockfd);
			process_client(clientfd);
			return 0;
		}
	}
}


