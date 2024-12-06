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
# include <fcntl.h>

# define PAGE_SIZE 4096
# define HTTP_SCHEME "http://"
# define HTTPS_SCHEME "https://"
# define HTTP_PORT "80"
# define HTTPS_PORT "443"

typedef struct s_URL {

    char    *port;
    char    *host;
    char    *uri;
    char    *url;

}   t_URL;

typedef struct s_HTTP_Response {

    int     status_code;
    char    *header;
    char    *body;
    int     is_chunked;
    long    content_len;

}   t_HTTP_Response;

typedef struct s_Opts {

    int     level;
    char    *path;
    t_URL   *url;

}   t_Opts;


typedef struct s_Socket {

    int     fd;
    SSL_CTX *ctx;
    SSL     *ssl;

}   t_Socket;


/* Parsing & Setting Options */
t_Opts      *ft_args(int argc, char **argv);
t_URL       *parse_url(char *url);


/* Creating & Sending the Request */
char        *ft_network(const t_URL *url);
int    create_socket(char* host, char *port);
bool    set_blocking_mode(int fd, bool block);
char    init_https(t_Socket *socket, char *host);
void        parse_html(const char *response);

void        ft_SSL_init();
void        ft_SSL_free(SSL *ssl, SSL_CTX *ctx);

// char        *get_ipv4(const char *domain_name);
void        *get_ipv4(const char *domain_name);
char    *get_response(t_Socket *socket, char* port);
int read_socket(t_Socket *socket, char *buffer, size_t size, char *port);

bool        start_with(char *url, char *reference);

t_HTTP_Response *parse_http_response(const char *response);




#endif