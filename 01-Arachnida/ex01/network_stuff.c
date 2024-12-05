#include "spider.h"
#include "fcntl.h"


#include <string.h>
#include <stdlib.h>
# include <arpa/inet.h>
# include <netdb.h>


char* extract_cookies(const char* response) {
    const char* set_cookie_start = "Set-Cookie: ";
    const char* line_start = response;
    char* cookies = malloc(1); // Start with empty string
    cookies[0] = '\0'; // Null-terminate

    while ((line_start = strstr(line_start, set_cookie_start)) != NULL) {
        line_start += strlen(set_cookie_start); // Skip "Set-Cookie: "
        const char* line_end = strstr(line_start, "\r\n");
        if (!line_end) break; // No more headers
        
        size_t cookie_length = line_end - line_start;
        char* cookie = malloc(cookie_length + 1);
        strncpy(cookie, line_start, cookie_length);
        cookie[cookie_length] = '\0';

        // Append to cookies string
        size_t current_length = strlen(cookies);
        cookies = realloc(cookies, current_length + cookie_length + 3); // Add "; " and null terminator
        strcat(cookies, cookie);
        strcat(cookies, "; "); // Cookies are separated by "; "
        free(cookie);

        line_start = line_end; // Move to the next line
    }
    return cookies;
}

t_HTTP_Response *parse_http_response(const char *response) {

    t_HTTP_Response *res_parsed = (t_HTTP_Response *)malloc(sizeof(t_HTTP_Response));

    //Protection

    res_parsed->header = NULL;
    res_parsed->body = NULL;
    res_parsed->is_chunked = 0;
    res_parsed->content_len = -1; 
    
    const char *header_end = strstr(response, "\r\n\r\n"); // The header ends with \r\n\r\n

    if (!header_end) {
        printf("error in parsing header");
        free(header_end);
        return NULL;
    }

    size_t header_len = (size_t)(header_end - response);

    res_parsed->header = strndup(response, header_len);


    printf("size of response: %s\n", res_parsed->header);
    

    free(res_parsed);
    free(response);


}

char set_blocking_mode(int fd, char blocking)
{
   if (fd < 0) return 0;

   int flags = fcntl(fd, F_GETFL, 0);
   if (flags == -1) return 0;
   flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
   return (fcntl(fd, F_SETFL, flags) == 0) ? 1 : 0;

}

char timeout_connect(int fd, const struct sockaddr* name, int namelen, int timeout_sec)
{
    fd_set fdset;
    struct timeval tv;
    int r;

    set_blocking_mode(fd, 0);
    connect(fd, name, namelen);
    set_blocking_mode(fd, 1); 

    FD_ZERO(&fdset);
    FD_SET(fd, &fdset);
    tv.tv_sec = timeout_sec;
    tv.tv_usec = 0;
    r = select(fd + 1, NULL, &fdset, NULL, &tv) == 1;
    return r;
}

int    create_socket(char* host, int port) {
    int sock_fd;
    struct addrinfo hints;
    struct addrinfo *res;

    hints.ai_family = AF_INET;       
    hints.ai_socktype = SOCK_STREAM;   
    hints.ai_flags = 0;
    hints.ai_protocol = IPPROTO_TCP;  

    getaddrinfo(host, "443", &hints, &res);

    while (res != NULL) {
        sock_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sock_fd == -1) {
            continue;
        }
        if (timeout_connect(sock_fd, res->ai_addr, res->ai_addrlen, 10)) {
            break;
        }
        close(sock_fd);
        res = res->ai_next;
    }
    freeaddrinfo(res);


    return sock_fd;
}

char    *ft_response(SSL *ssl) {

    char    buffer[PAGE_SIZE];
    int     bytes_received;
    size_t  total_size = 0;
    char    *response;

    response = malloc(PAGE_SIZE * sizeof(char));

    // Protection

    while ((bytes_received = SSL_read(ssl, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_received] = '\0';  
        char *new_res = realloc(response, total_size + bytes_received);
        if (!new_res) {
            // free(response);

            return NULL;
        }
        response = new_res;
        memcpy(response + total_size, buffer, bytes_received);
        total_size += bytes_received;
        response[total_size] = '\0';
    }

    if (bytes_received == -1) {
        perror("recv");
        return NULL;
    }


    return response;
}

char    *ft_request(t_url *url) {

    t_socket    *socket;
    char        *response;
    
    ft_SSL_init();

    socket = (t_socket *)malloc(sizeof(t_socket));

    socket->fd = -1;
    socket->ctx = NULL;
    socket->ssl = NULL;

    socket->fd = create_socket(url->host, url->port); // Creation of socket
    const SSL_METHOD *method = TLS_client_method();
    
    socket->ctx = SSL_CTX_new(method); // Init ssl Context -> Client
    
    if (!socket->ctx) {
        fprintf(stderr, "Eroor");
        close(socket->fd);
        free(socket);
    }

    // SSL_CTX_set_options(socket->ctx, SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3);

    socket->ssl = SSL_new(socket->ctx);

    if (!socket->ssl) {
        SSL_CTX_free(socket->ctx);
        close(socket->fd);
        free(socket);
    }

    SSL_set_tlsext_host_name(socket->ssl, "github.com");
    SSL_set_fd(socket->ssl, socket->fd);

    if (SSL_connect(socket->ssl) <= 0) {
        ft_SSL_free(socket->ssl, socket->ctx);
        close(socket->fd);
        free(socket);
        exit(1);
    }

    const char* request = "GET / HTTP/1.1\r\n"
                          "Host:  github.com \r\n"
                          "User-Agent: curl/8.4.0\r\n"
                          "Accept: */*\r\n"                          
                          "Accept-Encoding: encoding\r\n"
                          "Connection: close\r\n\r\n";

    
    // snprintf(request, sizeof(request), 
    //     "GET / HTTP/1.1\r\n"
    //     "Host:  stackoverflow.com \r\n"
    //     "Content-Length: 0\r\n"
    //     "Content-Type: application/x-www-form-urlencoded\r\n"
    //     "Accept: */*\r\n"
    //     "User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/51.0.2704.103 Safari/537.36\r\n"
    //     "Connection: close\r\n"
    //     "Accept-Encoding: identity\r\n\r\n"
    //     "\r\n");
        
        // "GET / HTTP/1.1\r\n"
        // "Host: stackoverflow.com\r\n"
        // "Content-Length: 0\r\n"
        // "Content-Type: application/x-www-form-urlencoded\r\n"
        // "Accept: */*\r\n"
        // "Content-Length: 0\r\n"
        // "User-Agent: curl/8.4.0\r\n"
        // "Connection: keep-alive\r\n"
        // "Accept-Encoding: identity\r\n"
    SSL_write(socket->ssl, request, strlen(request));

    if ( !(response = ft_response(socket->ssl))) {
        ft_SSL_free(socket->ssl, socket->ctx);
        close(socket->fd);
        free(socket);
        exit(1); 
    }
    printf("%s", response);
    //parse response
    // parse_http_response(response);
    SSL_shutdown(socket->ssl);
    ft_SSL_free(socket->ssl, socket->ctx);
    close(socket->fd);
    
    free(socket);
    free(url->uri);
    free(url->host);
    // free(url->ipv4);
    free(url);
    
    return response;
}