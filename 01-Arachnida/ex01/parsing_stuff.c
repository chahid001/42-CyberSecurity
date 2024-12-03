#include "spider.h"

bool    start_with(char *url, char *reference) {

    int i = 0;

    while (url[i] && reference[i] && i < strlen(reference)) {
        if (url[i] == reference[i]) {
            i++;
        }
        else {
            return 0;
        }
    }
    return 1;
}

t_url   *parse_url(char *url) {

    t_url *parsed_url;

    parsed_url = (t_url *)malloc(sizeof(t_url));

    parsed_url->url = url; //Reference URL

    // Check & Pass port & Scheme
    if (start_with(url, HTTP_SCHEME)) {
        parsed_url->port = HTTP_PORT;
        url += 7;
    } else if (start_with(url, HTTPS_SCHEME)) {
        parsed_url->port = HTTPS_PORT;
        url += 8;
    } else {
        printf("error: not valid url\n");
        // free(parsed_url);
        exit(1);
    }

    // Check & pass URI
    char *pos = strchr(url, '/');
    size_t len_pos = 0;
    size_t len_host;
    if (pos) {
        len_pos = strlen(pos);
        parsed_url->uri = (char *)malloc((len_pos + 1) * sizeof(char));
        if (!parsed_url->uri) {
            //error
        }
        strcpy(parsed_url->uri, pos);
    } else {
        parsed_url->uri = (char *)malloc(sizeof(char) + 1);
        if (!parsed_url->uri) {
            //error
        }
        parsed_url->uri[0] = '/';
        parsed_url->uri[1] = '\0';
    }

    // Host URL
    len_host = (strlen(url) - len_pos) + 1;
    if (!start_with(url, "www")) {
        len_host += 4; // Space needed for 'www.'
    }
    

    parsed_url->host = (char *)malloc((len_host * sizeof(char)) + 1);
    //protection

    if (start_with(url, "www")) {
        strlcpy(parsed_url->host, url,  len_host);
    } else {
        // Copying only the url ( minus 4 of www. and 1 for \0 -> 5)
        snprintf(parsed_url->host, len_host, "www.%.*s", len_host - 5, url);
    }

    // protection
    parsed_url->ipv4 = get_ipv4(parsed_url->host); // optimise memory
    // testing
    printf("REF_URL: %s\n", parsed_url->url);
    printf("URL: %s\n", parsed_url->host);
    printf("URI: %s\n", parsed_url->uri);
    printf("PORT: %d\n", parsed_url->port);
    printf("IP: %s\n", parsed_url->ipv4);

    return parsed_url;
}

// void    parse_html(const char *html_source) {

//     const char *img_tag = "<img ";
//     const char *src_attribute = "src=\"";

//     char* pos = html_source;
//     // while(html_source) {
//     while ((pos = strstr(pos, img_tag)) != NULL) {

//         char* src_start = strstr(pos, src_attribute);
//         if (src_start) {
//             src_start += strlen(src_attribute); // pass src="
//             char* src_end = strchr(src_start, '"');

//             if (src_end) {
//                 size_t len = (size_t)(src_end - src_start);
//                 char *img_path = malloc((len * sizeof(char)) + 1);
//                 memcpy(img_path, src_start, len);
//                 img_path += '\0';
//                 printf("srcsss: %s\n", img_path);
//                 free(img_path);
//             }
//         }
//         pos++;
//     }
        
        
        
        
        
        
        
//         // printf("srcsss: %s\n", img_path);
//         // printf("/////////////\n");

//         // printf("size: %d\n", strlen(src_start));
//         // printf("size: %d\n", strlen(src_end));
//     //     html_source++;
//     // }

// }