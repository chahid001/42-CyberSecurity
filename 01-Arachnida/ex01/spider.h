#ifndef SPIDER_H
# define SPIDER_H

# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <ctype.h>
# include <netdb.h>
# include <string.h>
# include <arpa/inet.h>
# include <openssl/ssl.h>
# include <openssl/err.h>
# include <stdbool.h>

# define PAGE_SIZE 4096
# define HTTP_SCHEME "http://"
# define HTTPS_SCHEME "https://"
# define HTTP_PORT 80
# define HTTPS_PORT 443

typedef struct s_url {

    int     port;
    char    *host;
    char    *uri;
    char    *url;
    char    *ipv4;

}   t_url;


typedef struct s_opts {

    int     level;
    char    *path;
    t_url   *url;

}   t_opts;


typedef struct s_socket {

    int     fd;
    SSL_CTX *ctx;
    SSL     *ssl;

}   t_socket;

t_opts      *ft_args(int argc, char **argv);

t_url       *parse_url(char *url);


void        ft_SSL_init();
void        ft_SSL_free(SSL *ssl, SSL_CTX *ctx);

int         create_socket(char* ipv4, int port);

char        *get_ipv4(const char *domain_name);


char    *ft_response(SSL *ssl);
char    *ft_request(t_url *url);
void    parse_html(const char *html_source);

bool    start_with(char *url, char *reference);
#endif