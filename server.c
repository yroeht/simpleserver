#include <errno.h>
#include <netinet/ip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
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

static struct timespec started_time;

void start()
{
	clock_gettime(CLOCK_REALTIME, &started_time);
}

void stop(int fd)
{
	struct timespec new_time;
	clock_gettime(CLOCK_REALTIME, &new_time);

	struct timespec elapsed;
	elapsed.tv_sec = new_time.tv_sec - started_time.tv_sec;
	elapsed.tv_nsec = new_time.tv_nsec - started_time.tv_nsec;
	if (new_time.tv_nsec < started_time.tv_nsec)
	{
		elapsed.tv_nsec += 1e9;
		elapsed.tv_sec -= 1;
	}

	char buffer[64];
	snprintf(buffer, sizeof(buffer), "%lu s %lu ns",
			elapsed.tv_sec,
			elapsed.tv_nsec);
	write(fd, buffer, strlen(buffer));
}

static
void process_client(int fd)
{
	char buffer[16] = { 0 };

	read(fd, buffer, sizeof(buffer));

	if (!strcmp(buffer, "start"))
		start();
	if (!strcmp(buffer, "stop"))
		stop(fd);
}

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s <port>", argv[0]);
		return -1;
	}

	int port = atoi(argv[1]);
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

		process_client(clientfd);
		close(clientfd);
	}
}


