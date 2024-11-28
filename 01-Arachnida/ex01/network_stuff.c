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

int    create_socket(const char* ipv4) {
    int sock_fd;
    struct sockaddr_in target;


    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        perror("socket");
        return -1;
    }

    target.sin_family = AF_INET;
    target.sin_port = htons(80);
    target.sin_addr.s_addr = inet_addr(ipv4);


    if (connect(sock_fd, (struct sockaddr *)&target, sizeof(target)) == -1) {
        perror("Failed to connect.");
        close(sock_fd);
        return -1;
    }

    return sock_fd;
}

