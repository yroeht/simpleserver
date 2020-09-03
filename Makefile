all: server client

CFLAGS=-Wall -Wextra

server:
	gcc $(CFLAGS) -o server server.c

libclient:
	gcc $(CFLAGS) -shared -o libclient.so -fpic libclient.c

client: libclient
	gcc $(CFLAGS) -L . -o client client.c -lclient

clean:
	rm libclient.so
	rm client
	rm server
