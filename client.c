#include <errno.h>
#include <netdb.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char** argv)
{
	if (argc < 3)
	{
		fprintf(stderr, "Usage: %s <address> <port>", argv[0]);
		return -1;
	}

	int port = atoi(argv[2]);
	if (port < 1 || port > 65535)
	{
		fprintf(stderr, "invalid port number %d", port);
		return -1;
	}

	struct hostent *server = gethostbyname(argv[1]);
	if (!server)
	{
		fprintf(stderr, "no such hostname %s", argv[1]);
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
}
