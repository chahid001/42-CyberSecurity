#include "spider.h"

char *get_ipv4(const char *domain_name) {
    struct addrinfo hints, *res;

    int status;
    char *ipv4_str = (char *)malloc(INET_ADDRSTRLEN);

    if (!ipv4_str) {
        return NULL;
    }

    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    status = getaddrinfo(domain_name, NULL, &hints, &res);

    if (status != 0) {
        fprintf(stderr, "Error: %s", gai_strerror(status));
        return NULL;
    }

    void *addr;
    struct sockaddr_in *ipv4 = (struct sockaddr_in*)res->ai_addr;
    addr = &(ipv4->sin_addr);

    inet_ntop(AF_INET, addr, ipv4_str, INET_ADDRSTRLEN);
    freeaddrinfo(res);


    printf("add: %s\n", ipv4_str);

    return ipv4_str;

}

int    create_socket(char* ipv4) {
    int sock_fd;
    struct sockaddr_in target;


    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        perror("socket");
        return -1;
    }

    target.sin_family = AF_INET;
    target.sin_port = htons(443);
    target.sin_addr.s_addr = inet_addr(ipv4);


    if (connect(sock_fd, (struct sockaddr *)&target, sizeof(target)) == -1) {
        perror("Failed to connect.");
        close(sock_fd);
        return -1;
    }
    free(ipv4);

    return sock_fd;
}

char* ft_http(int fd, char *domain) {

    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

    SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
    
    if (!ctx) {
        fprintf(stderr, "Eroor");
        close(fd);
        return -1;
    }

    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, fd);

    if (SSL_connect(ssl) <= 0) {
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        close(fd);
        return -1;
    }


    char request[1024];

    snprintf(request, sizeof(request), 
        "GET / HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Connection: close\r\n"
        "\r\n", domain);

    SSL_write(ssl, request, strlen(request));

    
    size_t total_size = 0;
    size_t chunck_size = 4096;
    char *html_source = malloc(chunck_size * sizeof(char));

    if (!html_source) {
        return -1;
    }

    char buffer[4096];
    int bytes_received;
    int i = 0;
    while ((bytes_received = SSL_read(ssl, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_received] = '\0';  
        char *new_html = realloc(html_source, total_size + bytes_received);
        if (!new_html) {
            free(html_source);
            return -1;
        }
        html_source = new_html;
        memcpy(html_source + total_size, buffer, bytes_received);
        total_size += bytes_received;
        html_source[total_size] = '\0'; 
    }

    if (bytes_received == -1) {
        perror("recv");
        return -1;
    }
    // printf("%s", html_source);

    SSL_shutdown(ssl);
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    close(fd);
    // free(html_source);
    return html_source;
}