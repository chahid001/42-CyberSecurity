#include "spider.h"

// void    get_ipv4(const char *domain_name) {
//     struct addrinfo hints, *res;

//     int status;
//     // char *ipv4_str = (char *)malloc(INET_ADDRSTRLEN);

//     // if (!ipv4_str) {
//     //     return NULL;
//     // }

//     memset(&hints, 0, sizeof(hints));

//     hints.ai_family = AF_INET;
//     hints.ai_socktype = SOCK_STREAM;

//     status = getaddrinfo(domain_name, "443", &hints, &res);

//     if (status != 0) {
//         fprintf(stderr, "Error: %s", gai_strerror(status));
//         return NULL; 
//     }

//     // void *addr;
//     // struct sockaddr_in *ipv4 = (struct sockaddr_in*)res->ai_addr;
//     // addr = &(ipv4->sin_addr);

//     // inet_ntop(AF_INET, addr, ipv4_str, INET_ADDRSTRLEN);
//     // freeaddrinfo(res);

//     // return res;
// }