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


typedef enum {
    RESPONSE_TYPE_REDIRECTION,
    RESPONSE_TYPE_GENERIC,
    RESPONSE_TYPE_IMAGE,
}   ResponseType;


typedef struct s_Image {
    char    *img_type;
}   t_Image; 

typedef struct s_Redirection {

    char    *location;

}   t_Redirection;

typedef struct s_Generic {
    
    char    *body;
    bool    is_chunked;

}   t_Generic;

typedef struct s_Response {

    char            *header;
    ResponseType    type;

    union {

        t_Redirection   redirection_data;
        t_Generic       generic_data;
        t_Image         image_data;

    }   Content;

}   t_Response;


typedef struct s_URL {

    char    *port;
    char    *host;
    char    *uri;
    char    *url;

}   t_URL;

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

void free_them_all(t_Opts *opts, t_Socket *socket, t_Response *parsed_response, char *raw_response, bool flag);
/* Parsing & Setting Options */
t_Opts      *ft_args(int argc, char **argv);
t_URL       *parse_url(char *url);
void    download_stuff(t_URL *image_url, char *image_ext, int i);
char    send_request(const t_Socket *socket, const t_URL *url);
/* Creating & Sending the Request */
t_Socket    *ft_network(const t_URL *url);
int    create_socket(const char* host, const char *port);
bool    set_blocking_mode(int fd, bool block);
char    init_https(t_Socket *socket, const char *host);
char**        parse_html(const char *body);

void        ft_SSL_init();
void        ft_SSL_free(SSL *ssl, SSL_CTX *ctx);

char    *get_response(const t_Socket *socket, const char* port);
int read_socket(const t_Socket *socket, char *buffer, const size_t size, const char *port);
bool        start_with(char *url, char *reference);

t_Response *parse_http_response(const char *raw_response);

void    decode_body(char *encoded_body);
char* open_mind_strstr(const char* haystack, const char* needle);
bool    check_type_img(char *type);
int check_status_code(int status_code);

#endif