#ifndef LIBCLIENT_H
# define LIBCLIENT_H

void start(const char *hostname, const char *portstr);

char *stop(const char *hostname, const char *portstr);

#endif
