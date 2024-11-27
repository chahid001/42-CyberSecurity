#include "spider.h"

void get_ipv4(const char *domain_name) {
    struct addrinfo hints, *res;

    int status;
    char *ipv4 = (char *)malloc(INET_ADDRSTRLEN);

    if (!ipv4) {
        // return NULL;
    }

    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    status = getaddrinfo(domain_name, NULL, &hints, &res);

    if (status != 0) {
        fprintf(stderr, "Error: %s", gai_strerror(status));
        // return NULL;
    }
}