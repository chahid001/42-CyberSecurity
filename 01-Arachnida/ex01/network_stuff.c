#include "spider.h"

/*
    Network stuff, because it's too late now to return 
    to python. 

    Malloc: Socket obj, ipv4
*/


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
    if (!set_blocking_mode(fd, 1)) {
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

int    create_socket(char* host, char *port) {
    int             sock_fd;
    void            *addr;
    struct addrinfo hints;
    struct addrinfo *result;
    
    memset(&hints, 0, sizeof(hints));
    char *ipv4_str = (char *)malloc(INET_ADDRSTRLEN * sizeof(char));

    if (!ipv4_str) {
        perror("Malloc: Failed to allocate IPV4.\n");
        return NULL;
    }

    hints.ai_family = AF_INET; /* IPV4 */       
    hints.ai_socktype = SOCK_STREAM; /* TCP */         
    hints.ai_protocol = IPPROTO_TCP; /* TCP */ 

    if (getaddrinfo(host, port, &hints, &result) != 0) {
        perror("Error: Website not found.");
        exit(EXIT_FAILURE);
    } 


    struct sockaddr_in *ipv4 = (struct sockaddr_in*)result->ai_addr;
    addr = &(ipv4->sin_addr);


    while (result != NULL) {

        sock_fd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
        if (sock_fd == -1) {
            continue;
        }
        if (timeout_connect(sock_fd, result->ai_addr, result->ai_addrlen, 10)) {
            inet_ntop(result->ai_family, addr, ipv4_str, INET_ADDRSTRLEN); /* Converting IPV4 to string */
            printf("Connected to %s -> | %s | - port - %s - \n", host, ipv4_str, port);
            break;
        }
        close(sock_fd);
        result = result->ai_next;
    }
    freeaddrinfo(result);
    free(ipv4_str);
    return sock_fd;
}

/*
    Init SSL for https connections.
*/
char    init_https(t_Socket *socket, char *host) {

    ft_SSL_init();

    const SSL_METHOD *method = TLS_client_method();

    socket->ctx = SSL_CTX_new(method);

    if (!socket->ctx) {
        perror("Error ctx");
        close(socket->fd);
        return 1;
    }

    SSL_CTX_set_options(socket->ctx, SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3);

    socket->ssl = SSL_new(socket->ctx);

    if (!socket->ssl) {
        perror("Error ctx");
        SSL_CTX_free(socket->ctx);
        close(socket->fd);
        return 1;
    }

    SSL_set_tlsext_host_name(socket->ssl, host);
    SSL_set_fd(socket->ssl, socket->fd);

    if (SSL_connect(socket->ssl) <= 0) {
        perror("Error connect");
        ft_SSL_free(socket->ssl, socket->ctx);
        close(socket->fd);
        return 1;
    }
    printf ("Success\n");
    return 0;
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

void    send_request(t_Socket *socket, t_URL *url, char *SCHEME) {

    char *request[1024];

    snprintf(request, sizeof(request), 
        "GET %s HTTP/1.1\r\n"
        "Host: %s \r\n"
        "Content-Length: 0\r\n"
        "Content-Type: application/x-www-form-urlencoded\r\n"
        "Accept: */*\r\n"
        "User-Agent: curl/8.4.0\r\n"
        "Connection: close\r\n"
        "Accept-Encoding: identity\r\n\r\n"
        "\r\n", url->uri, url->host);

    if (SCHEME == HTTP_SCHEME) {

    } else if (SCHEME == HTTPS_SCHEME) {
        if (SSL_write(socket->ssl, request, strlen(request))) {
            printf("SSL request is succes.\n");
        }
    }

}

char    *ft_network(const t_URL *url) {

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


    socket->fd = create_socket(url->host, url->port);

    printf("Protocol: HTTPS\n");

    if (url->port == HTTP_PORT) {

    } else if (url->port == HTTPS_PORT) {
        
        

        if (init_https(socket, url->host) != 0) {
            perror("Couldn't initialise ssl.");
            free(socket);
        }

        send_request(socket, url, HTTPS_SCHEME);
        

    }

    // if ( !(response = ft_response(socket->ssl))) {
    //     ft_SSL_free(socket->ssl, socket->ctx);
    //     close(socket->fd);
    //     free(socket);
    //     exit(1); 
    // }
    // printf("%s", response);
    //parse response
    // parse_http_response(response);
    SSL_shutdown(socket->ssl);
    ft_SSL_free(socket->ssl, socket->ctx);
    close(socket->fd);
    
    free(socket);
    free(url->uri);
    free(url->host);
    free(url);
    
    // return response;
}