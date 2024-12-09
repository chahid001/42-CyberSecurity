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
        perror("Failed to set file descriptor flags.");
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

int    create_socket(const char* host, const char *port) {
    int             sock_fd;
    void            *addr;
    struct addrinfo hints;
    struct addrinfo *result;
    
    memset(&hints, 0, sizeof(hints));
    char *ipv4_str = (char *)malloc(INET_ADDRSTRLEN * sizeof(char));

    if (!ipv4_str) {
        perror("Malloc: Failed to allocate IPV4.\n");
        return -1;
    }

    hints.ai_family = AF_INET; /* IPV4 */       
    hints.ai_socktype = SOCK_STREAM; /* TCP */         
    hints.ai_protocol = IPPROTO_TCP; /* TCP */ 

    if (getaddrinfo(host, port, &hints, &result) != 0) { 
        perror("Error: Website not found.");
        free(ipv4_str);
        return -1;
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
char    init_https(t_Socket *socket, const char *host) {

    ft_SSL_init();

    const SSL_METHOD *method = SSLv23_client_method();

    socket->ctx = SSL_CTX_new(method);

    if (!socket->ctx) {
        perror("Error ctx");
        return 1;
    }

    SSL_CTX_set_options(socket->ctx, SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3);

    socket->ssl = SSL_new(socket->ctx);

    if (!socket->ssl) {
        perror("Error ctx");
        SSL_CTX_free(socket->ctx);
        return 1;
    }

    SSL_set_tlsext_host_name(socket->ssl, host);
    SSL_set_fd(socket->ssl, socket->fd);

    if (SSL_connect(socket->ssl) <= 0) {
        perror("Error connect");
        ft_SSL_free(socket->ssl, socket->ctx);
        return 1;
    }
    printf ("Success\n");
    return 0;
}

/*
    Read socket response base on Protocol Scheme.
    recv -> HTTP
    SSL_read -> HTTPS
*/
int read_socket(const t_Socket *socket, char *buffer, const size_t size, const char *port) {

    if (strcmp(port, HTTP_PORT) == 0) {

    } else if (strcmp(port, HTTPS_PORT) == 0) {
        return SSL_read(socket->ssl, buffer, size - 1);
    } 
    return 0; 
}     

/*
    Allocate the Raw response into a pointer.
*/
char    *get_response(const t_Socket *socket, const char* port) {

    char    buffer[PAGE_SIZE];
    int     bytes_received;
    size_t  total_size = 0;
    char    *response;

    response = malloc(PAGE_SIZE * sizeof(char));
    if (!response) {
        perror("Malloc: Failed to allocate response.\n");
        free(response);
        return NULL;
    }

    while ((bytes_received = read_socket(socket, buffer, PAGE_SIZE, port)) > 0) {
        buffer[bytes_received] = '\0';  
        
        char *new_res = realloc(response, total_size + bytes_received);
        
        if (!new_res) {
            perror("Realloc Failed.\n");
            free(response);
            return NULL;
        }
        response = new_res;
        memcpy(response + total_size, buffer, bytes_received);
        total_size += bytes_received;
        response[total_size] = '\0';
    }

    if (bytes_received == -1) {
        perror("Failed to pass response.\n");
        free(response);
        return NULL;
    }
    return response;
}


char    send_request(const t_Socket *socket, const t_URL *url) {

    char request[1024];

    snprintf(request, sizeof(request), 
        "GET %s HTTP/1.1\r\n"
        "Host: %s \r\n"
        "Content-Type: application/x-www-form-urlencoded\r\n"
        "Accept: */*\r\n"
        "User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/51.0.2704.103 Safari/537.36\r\n"
        "Connection: close\r\n"
        "Accept-Encoding: identity\r\n\r\n"
        "\r\n", url->uri, url->host);

    if (strcmp(url->port,HTTP_PORT) == 0) {

    } else if (strcmp(url->port,HTTPS_PORT) == 0) {
        if (SSL_write(socket->ssl, request, strlen(request))) {
            printf("SSL request is succes.\n");
        } else {
            perror("SSL failed.\n");
            return 1;
        }
    }
    
    return 0;
}


t_Socket    *ft_network(const t_URL *url) { // url obj, uri, host, Socket obj, 

    t_Socket    *socket;
    char        *raw_response;

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

    if (socket->fd == -1) {
        free(socket);
        return NULL;
    }

    printf("Protocol: HTTPS\n"); // change later for verbose

    if (strcmp(url->port, HTTP_PORT) == 0) {
        
        // http


    } else if (strcmp(url->port, HTTPS_PORT) == 0) {

        if (init_https(socket, url->host) != 0) {
            perror("Couldn't initialise ssl.");
            close(socket->fd);
            free(socket);
            return NULL;
        } 

    }

    if (send_request(socket, url) != 0) {
        close(socket->fd);
        free(socket);
        return (NULL);
    }
    
    // raw_response = get_response(socket, url->port);
    
    // if (!raw_response) {
    //     close(socket->fd);
    //     free(socket);
    //     return NULL;
    // }

    // SSL_shutdown(socket->ssl);
    // ft_SSL_free(socket->ssl, socket->ctx);
    // close(socket->fd);
    // free(socket);

    
    return socket;
}

void    download_stuff(t_URL *image_url, char *image_ext, int i) {
    
    t_Socket *socket = ft_network(image_url);

    char buffer[4096];
    int bytes_read;
    int header_received = 0;

    char *image_name = malloc((9 + strlen(image_ext)) * sizeof(char));

    if (!image_name) {
        fprintf(stderr, "Malloc: Failed allocating image name.\n");
        free_them_all(NULL, socket, NULL, NULL, NULL);
        exit(EXIT_FAILURE);
    }

    snprintf(image_name, (9 + strlen(image_ext)), "image-%d.%s", i++, image_ext);

    FILE *file = fopen(image_name, "wb");
    if (!file) {
        exit(EXIT_FAILURE);
    }

    while ((bytes_read = SSL_read(socket->ssl, buffer, sizeof(buffer))) > 0) {
        if (!header_received) {
            char *header_end = strstr(buffer, "\r\n\r\n");
            if (header_end) {
                header_received = 1;
                header_end += 4; // Skip past the header
                fwrite(header_end, 1, bytes_read - (header_end - buffer), file);
            }
        } else {
            
            fwrite(buffer, 1, bytes_read, file);
        }
    }
    free(image_name);
    free_them_all(NULL, socket, NULL, NULL, NULL);
}