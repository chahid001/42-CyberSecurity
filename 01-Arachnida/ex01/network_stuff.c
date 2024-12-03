#include "spider.h"

int    create_socket(char *ipv4, int port) {
    int sock_fd;
    struct sockaddr_in target;


    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        perror("socket");
        return -1;
    }

    target.sin_family = AF_INET;
    target.sin_port = htons(port);
    target.sin_addr.s_addr = inet_addr(ipv4);


    if (connect(sock_fd, (struct sockaddr *)&target, sizeof(target)) == -1) {
        perror("Failed to connect.");
        close(sock_fd);
        return -1;
    }

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
    char        request[1024];
    char        *response;
    
    ft_SSL_init();

    socket = (t_socket *)malloc(sizeof(t_socket));

    socket->fd = create_socket(url->ipv4, url->port); // Creation of socket
    socket->ctx = SSL_CTX_new(TLS_client_method()); // Init ssl Context -> Client
    
    if (!socket->ctx) {
        fprintf(stderr, "Eroor");
        close(socket->fd);
        free(socket);
    }

    socket->ssl = SSL_new(socket->ctx);

    if (!socket->ssl) {
        SSL_CTX_free(socket->ctx);
        close(socket->fd);
        free(socket);
    }

    SSL_set_tlsext_host_name(socket->ssl, url->host);
    SSL_set_fd(socket->ssl, socket->fd);

    if (SSL_connect(socket->ssl) <= 0) {
        ft_SSL_free(socket->ssl, socket->ctx);
        close(socket->fd);
        free(socket);
        exit(1);
    }

    snprintf(request, sizeof(request), 
        "GET %s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Accept: */*\r\n"
        "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/117.0.0.0 Safari/537.36\r\n"
        "Connection: close\r\n"
        "Accept-Encoding: identity\r\n"
        "\r\n", url->uri, url->host);

    SSL_write(socket->ssl, request, strlen(request));

    if ( !(response = ft_response(socket->ssl))) {
        ft_SSL_free(socket->ssl, socket->ctx);
        close(socket->fd);
        free(socket);
        exit(1); 
    }

    
    SSL_shutdown(socket->ssl);
    ft_SSL_free(socket->ssl, socket->ctx);
    close(socket->fd);
    
    free(socket);
    free(url->uri);
    free(url->host);
    free(url->ipv4);
    free(url);
    
    return response;
}