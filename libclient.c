#include "libclient.h"
#include <errno.h>
#include <netdb.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static
int connect_client(const char *hostname, const char *portstr)
{
	int port = atoi(portstr);
	if (port < 1 || port > 65535)
	{
		fprintf(stderr, "invalid port number %d", port);
		return -1;
	}

	struct hostent *server = gethostbyname(hostname);
	if (!server)
	{
		fprintf(stderr, "no such hostname %s", hostname);
		return -1;
	}

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
	bcopy((char *)server->h_addr, (char *)&addr.sin_addr.s_addr, server->h_length);
	addr.sin_port = htons(port);

	if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
	{
		fprintf(stderr, "error connecting socket, %s",
				strerror(errno));
		return -1;
	}

	return sockfd;
}

void start(const char *hostname, const char *portstr)
{
	int sockfd = connect_client(hostname, portstr);
	if (sockfd < 0)
		return;

	const char *startstr = "start";

	write(sockfd, startstr, strlen(startstr));
	printf(" -> start\n");
	close(sockfd);
}


char *stop(const char *hostname, const char *portstr)
{
	int sockfd = connect_client(hostname, portstr);
	if (sockfd < 0)
		return NULL;

	const char *stopstr = "stop";

	write(sockfd, stopstr, strlen(stopstr));
	printf(" -> stop\n");

	char buffer[128];
	int nbbytes;
	nbbytes = read(sockfd, buffer, sizeof(buffer));
	printf("received %d bytes: %s", nbbytes, buffer);

	close(sockfd);
	return strdup(buffer);
}
