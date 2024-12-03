#include "spider.h"


void    ft_SSL_init() {

    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

}

void    ft_SSL_free(SSL *ssl, SSL_CTX *ctx) {
    
    SSL_free(ssl);
    SSL_CTX_free(ctx);
}