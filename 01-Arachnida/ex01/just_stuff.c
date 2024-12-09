#include "spider.h"

void free_them_all(t_Opts *opts, t_Socket *socket, t_Response *parsed_response, char *raw_response, bool flag) {
    
    if (raw_response) free(raw_response);

    if (opts) {
        if (opts->url->uri) free(opts->url->uri);
        if (opts->url->host) free(opts->url->host);
        if (flag) free(opts);
    }

    if (socket) {
        if (socket->ssl && socket->ctx) {
            SSL_shutdown(socket->ssl);
            ft_SSL_free(socket->ssl, socket->ctx);
        }
        if (socket->fd) close(socket->fd);
        free(socket);
    }

    if (parsed_response) {

        if (parsed_response->header) free(parsed_response->header);

        if (parsed_response->type == RESPONSE_TYPE_REDIRECTION) {

            if (parsed_response->Content.redirection_data.location) free(parsed_response->Content.redirection_data.location);
        }

        if (parsed_response->type == RESPONSE_TYPE_IMAGE) {

            if (parsed_response->Content.image_data.img_type) free(parsed_response->Content.image_data.img_type);
        }

        if (parsed_response->type == RESPONSE_TYPE_GENERIC) {

            if (parsed_response->Content.generic_data.body) free(parsed_response->Content.generic_data.body);
        }

        free(parsed_response);
    }
}

char* open_mind_strstr(const char* haystack, const char* needle) {

    if (!haystack || !needle) {
        return NULL;
    }

    size_t needle_len = strlen(needle);
    size_t haystack_len = strlen(haystack);

    for (size_t i = 0; i <= haystack_len - needle_len; i++) {
        size_t j = 0;
        while (j < needle_len && tolower((unsigned char)haystack[i + j]) == tolower((unsigned char)needle[j])) {
            j++;
        }
        if (j == needle_len) {
            return (char*)&haystack[i];
        }
    }
    return NULL; 
}

bool    check_type_img(char *type) {

    const char  *supported_types[] = { "png", "jpg", "jpeg", "gif", "bmp"};
    size_t  size_types = sizeof(supported_types) / sizeof(supported_types[0]);

    for (int i = 0; i < size_types; i++) {
        if (strcasecmp(type, supported_types[i]) == 0) {
            return true;
        }
    }

    return false;
}

int check_status_code(int status_code) {

    if (status_code == 200) {
        printf("Status code: %d --- OK\n", status_code);
        return 0;
    } else if (status_code == 301 || status_code == 302) {
        printf("Status code: %d --- Redirection\n", status_code);
        return 1;
    } else {
        printf("Status code: %d --- KO\n", status_code);
        return -1;        
    }
}