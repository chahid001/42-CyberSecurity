#include "spider.h"

/*
    Network stuff, because it's too late now to return 
    to python. 

    Malloc: Socket obj, ipv4
*/


// t_HTTP_Response *parse_http_response(const char *response) {

//     t_HTTP_Response *res_parsed = (t_HTTP_Response *)malloc(sizeof(t_HTTP_Response));

//     //Protection

//     res_parsed->header = NULL;
//     res_parsed->body = NULL;
//     res_parsed->is_chunked = 0;
//     res_parsed->content_len = -1; 
    
//     const char *header_end = strstr(response, "\r\n\r\n"); // The header ends with \r\n\r\n

//     if (!header_end) {
//         printf("error in parsing header");
//         free(header_end);
//         return NULL;
//     }

//     size_t header_len = (size_t)(header_end - response);

//     res_parsed->header = strndup(response, header_len);


//     printf("size of response: %s\n", res_parsed->header);
    

//     free(res_parsed);
//     free(response);


// }

/*
    Set blocking mode 
    | 0 -> non-blocking
    | 1 -> blocking 
*/
bool    set_blocking_mode(int fd, bool block) {
    
    if (fd < 0) {
        perror("Invalid file descriptor");
        return false;
    }
    int flags = fcntl(fd, F_GETFL);
    if (flags == -1) {
        perror("Failed to get file descriptor flags");
        return false;
    }
    if (block) {
        flags &= ~O_NONBLOCK; /* reversing O_NONBLOCK (Blocking) then setting flags to blocking mode */
    } else {
        flags |= O_NONBLOCK; /* Setting Non blocking mode */
    }

    if (fcntl(fd, F_SETFL, flags) == 0) {
        return true;
    } else {
        perror("Failed to set file descriptor flags");
        return false;
    }
}

/*
    Making the socket non blocking, connect, block it again, then monitore it with select
    with a timeout (by default 10 sec) before valide it or going to the next socket.
*/

char timeout_connect(int fd, const struct sockaddr* ipv4, int len, int timeout_sec)
{
    fd_set FDset;
    struct timeval tv;
    int ret;

    if (!set_blocking_mode(fd, 0)) {
        perror("FD: Failing to switch Blocking mode.");
    }
    connect(fd, ipv4, len);
    if (set_blocking_mode(fd, 1)) {
        perror("FD: Failing to switch Blocking mode.");
    }

    FD_ZERO(&FDset);
    FD_SET(fd, &FDset);
    tv.tv_sec = timeout_sec;
    tv.tv_usec = 0;
    ret = select(fd + 1, NULL, &FDset, NULL, &tv) == 1;
    return ret;
}

/*
    Finding the appropriate IPV4 for the host, 
    iterating thought the result linked list and connect to each FD
    with a timeout untill we do connect succesfully.
*/

int    create_socket(char* host, char *port, char *ipv4) {
    int sock_fd;
    struct addrinfo hints;
    struct addrinfo *result;
    
    memset(&hints, 0, sizeof(hints));
    ipv4 = (char *)malloc(INET_ADDRSTRLEN * sizeof(char));

    if (!ipv4) {
        perror("Malloc: Failed to allocate IPV4.\n");
        return NULL;
    }

    hints.ai_family = AF_INET; /* IPV4 */       
    hints.ai_socktype = SOCK_STREAM; /* TCP */         
    hints.ai_protocol = IPPROTO_TCP; /* TCP */ 

    getaddrinfo(host, port, &hints, &result); 

    while (result != NULL) {

        sock_fd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
        if (sock_fd == -1) {
            continue;
        }
        if (timeout_connect(sock_fd, result->ai_addr, result->ai_addrlen, 10)) {
            inet_ntop(AF_INET, result->ai_addr, ipv4, sizeof(ipv4)); /* Converting IPV4 to string */
            printf("Connected to %s -> | %s | - port | %s | \n", host, ipv4, port);
            break;
        }
        close(sock_fd);
        result = result->ai_next;
    }
    freeaddrinfo(result);

    return sock_fd;
}

// char    *ft_response(SSL *ssl) {

//     char    buffer[PAGE_SIZE];
//     int     bytes_received;
//     size_t  total_size = 0;
//     char    *response;

//     response = malloc(PAGE_SIZE * sizeof(char));

//     // Protection

//     while ((bytes_received = SSL_read(ssl, buffer, sizeof(buffer) - 1)) > 0) {
//         buffer[bytes_received] = '\0';  
//         char *new_res = realloc(response, total_size + bytes_received);
//         if (!new_res) {
//             // free(response);

//             return NULL;
//         }
//         response = new_res;
//         memcpy(response + total_size, buffer, bytes_received);
//         total_size += bytes_received;
//         response[total_size] = '\0';
//     }

//     if (bytes_received == -1) {
//         perror("recv");
//         return NULL;
//     }


//     return response;
// }

char    *ft_request(const t_URL *url) {

    t_Socket    *socket;
    // char        *response;

    socket = (t_Socket *)malloc(sizeof(t_Socket));

    if (!socket) {
        perror("Malloc: Failed to allocate Socket object.\n");
        return NULL;
    }

    /* Initialise Socket object */
    socket->fd = -1;
    socket->ctx = NULL;
    socket->ssl = NULL;


    socket->fd = create_socket(url->host, url->port, url->ipv4);
    


    
    // ft_SSL_init(); /* Initialise SSL */

    



     // Creation of socket
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