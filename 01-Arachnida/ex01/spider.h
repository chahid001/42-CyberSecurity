#ifndef SPIDER_H
# define SPIDER_H

# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <ctype.h>
# include <netdb.h>
# include <string.h>
# include <arpa/inet.h>


char        *get_ipv4(const char *domain_name);
int         create_socket(const char* ipv4);
#endif