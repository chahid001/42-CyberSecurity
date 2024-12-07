#include "spider.h"

/*
    Parsing stuff because parsing is cool.
*/

// malloc:  obj, uri & host.

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

t_URL   *parse_url(char *url) {

    t_URL *parsed_url;

    parsed_url = (t_URL *)malloc(sizeof(t_URL));
    parsed_url->url = url; // Reference URL
    parsed_url->port = NULL; // Port
    /*
        Checking the Scheme of the URL, to decide were to assign it.
        In case of HTTP 80 we work with socket functions.
        In case of HTTPS 443 we work with SSL functions.
    */

    if (start_with(url, HTTP_SCHEME)) { // Scheme
        parsed_url->port = HTTP_PORT;
        url += 7;
    } else if (start_with(url, HTTPS_SCHEME)) {
        parsed_url->port = HTTPS_PORT;
        url += 8;
    } else {
        printf("error: not valid url\n");
        free(parsed_url);
        exit(1);
    }

    /* 
        Checking if the URL has a URI, if not it will be assigned
        directly a '/' URI.
        We check the len to allocate exactly the len of URI.
    */

    char *pos = strchr(url, '/');
    size_t len_pos = 0;
    size_t len_host;
    if (pos) { // URI
        len_pos = strlen(pos);
        parsed_url->uri = (char *)malloc((len_pos + 1) * sizeof(char));

        if (!parsed_url->uri) {
            write(2, "Malloc: Failed to allocate URI.\n", 32);
            return NULL;
        }

        strcpy(parsed_url->uri, pos);

    } else {
        parsed_url->uri = (char *)malloc(sizeof(char) + 1);

        if (!parsed_url->uri) {
            write(2, "Malloc: Failed to allocate URI.\n", 32);
            return NULL;
        }

        parsed_url->uri[0] = '/';
        parsed_url->uri[1] = '\0';
    }

    
    /* 
        Checking if the URL start with 'www', if yes we need to remove it to keep 
        the host wirh root domain. In case of redirection it will be added
        automaticaly with Location header.
    */

    if (start_with(url, "www")) { // Host
        url += 4; 
        len_host = (strlen(url) - len_pos) + 1;
        parsed_url->host = (char *)malloc((len_host * sizeof(char)) + 1);

        if (!parsed_url->url) {
            write(2, "Malloc: Failed to allocate URL.\n", 32);
            return NULL;
        }

        strlcpy(parsed_url->host, url,  len_host);
    } else {
        len_host = (strlen(url) - len_pos) + 1;
        parsed_url->host = (char *)malloc((len_host * sizeof(char)) + 1);

        if (!parsed_url->url) {
            write(2, "Malloc: Failed to allocate URL.\n", 32);
            return NULL;
        }
        strlcpy(parsed_url->host, url,  len_host);
    }

    /* Testing */
    printf("REF_URL: %s\n", parsed_url->url);
    printf("HOST: %s\n", parsed_url->host);
    printf("URI: %s\n", parsed_url->uri);
    printf("PORT: %s\n", parsed_url->port);

    return parsed_url;
}


// void    parse_html(const char *response) {

//     const char *img_tag = "<img ";
//     const char *src_attribute = "src=\"";

//     char* pos = response;

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
//                 printf("Image: %s\n", img_path);
//                 free(img_path);
//             }
//         }
//         pos++;
//     }
// }       
        
        
        
t_HTTP_Response *parse_http_response(const char *raw_response) {

    t_HTTP_Response *res_parsed = (t_HTTP_Response *)malloc(sizeof(t_HTTP_Response));

    if (!res_parsed) {
        perror("Malloc: Failed to allocate res_parsed.\n");
        return NULL;
    }

    res_parsed->header = NULL;
    res_parsed->body = NULL;
    res_parsed->is_chunked = 0;
    res_parsed->content_len = -1; 
    


    /* Header */
    const char *header_end = strstr(raw_response, "\r\n\r\n"); // The header ends with \r\n\r\n

    if (!header_end) {
        printf("Error in parsing header.\n");
        return NULL;
    }
    res_parsed->header = strndup(raw_response, (size_t)(header_end - raw_response));
    /* end Header */

    /* Status Code */
    const char *status_code_end = strstr(raw_response, "\r\n"); /* First line */
    if (!status_code_end) {
        printf("Error in parsing status code.\n");
        return NULL;
    }
    char *status_line = strndup(raw_response, (status_code_end - raw_response));
    int status_code = 0;

    sscanf(status_line, "HTTP/%*s %d", &status_code);
    free(status_line);
    check_status_code(status_code);
    

    /* End Status Code*/
    
    printf("Header: \n%s\n", res_parsed->header);
    


    
    free(res_parsed->header);
    free(res_parsed);
}