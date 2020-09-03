#include "libclient.h"

#include <stdio.h>

int main(int argc, char **argv)
{
	if (argc < 3)
	{
		fprintf(stderr, "Usage: %s <address> <port>", argv[0]);
		return -1;
	}

	//sleep(rand() % 5);
	start(argv[1], argv[2]);
	//sleep(rand() % 5);
	stop(argv[1], argv[2]);

	return 0;
}
