#include <errno.h>
#include <netdb.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static
int connect_client(char *hostname, char *portstr)
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

int main(int argc, char **argv)
{
	if (argc < 3)
	{
		fprintf(stderr, "Usage: %s <address> <port>", argv[0]);
		return -1;
	}

	int sockfd = connect_client(argv[1], argv[2]);
	if (sockfd < 0)
		return -1;

	const char *startstr = "start";
	const char *stopstr = "stop";

	write(sockfd, startstr, strlen(startstr));
	printf(" -> start\n");
	close(sockfd);

	//sleep(rand() % 5);
	sockfd = connect_client(argv[1], argv[2]);
	if (sockfd < 0)
		return -1;

	//sleep(rand() % 5);

	write(sockfd, stopstr, strlen(stopstr));
	printf(" -> stop\n");

	char buffer[128];
	int nbbytes;
	do {
		nbbytes = read(sockfd, buffer, sizeof(buffer));
	} while (!nbbytes);
	printf("received %d bytes: %s", nbbytes, buffer);

	return 0;
}
