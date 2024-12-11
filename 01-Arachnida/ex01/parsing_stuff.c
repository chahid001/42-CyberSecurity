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


char    **parse_html(const char *body) {

    const char *img_tag = "<img ";
    const char *src_attribute = "src=\"";

    char* pos = body;

    char **image_links = NULL;
    size_t  i = 0;

    while ((pos = strstr(pos, img_tag)) != NULL) {

        char* src_start = strstr(pos, src_attribute);
        if (src_start) {
            src_start += strlen(src_attribute); // pass src="
            char* src_end = strchr(src_start, '"');

            if (src_end) {
                size_t len = (size_t)(src_end - src_start);
                char *img_path = malloc((len * sizeof(char)) + 1);
                memcpy(img_path, src_start, len);
                img_path[len] = '\0';
                
                char **temp = realloc(image_links, (i + 1) * sizeof(char *));
                if (!temp) {
                    fprintf(stderr, "Memory reallocation failed\n");
                    free(img_path);
                    break;
                }
                image_links = temp;
                image_links[i++] = img_path;
            }
        }
        pos++;
    }
    return image_links;
}       
        
t_Response *parse_http_response(const char *raw_response, bool flag) {

    t_Response *res_parsed = (t_Response *)malloc(sizeof(t_Response));

    if (!res_parsed) {
        perror("Malloc: Failed to allocate res_parsed.\n");
        return NULL;
    }
    

    /* Header */
    const char *header_end = strstr(raw_response, "\r\n\r\n"); // The header ends with \r\n\r\n

    if (!header_end) {
        printf("Error in parsing header.\n");
        return NULL;
    }
    res_parsed->header = strndup(raw_response, (size_t)(header_end - raw_response));
    /* end Header */


    /* Status Code */
    const char *status_code_end = strstr(res_parsed->header, "\r\n"); /* First line */
    
    if (!status_code_end) {
        printf("Error in parsing status code.\n");
        return NULL;
    }
    char *status_line = strndup(res_parsed->header, (status_code_end - res_parsed->header));
    int status_code = 0;

    sscanf(status_line, "HTTP/%*s %d", &status_code);
    free(status_line);
    if (check_status_code(status_code) == -1) {
        printf("Status code: %d --- KO\n", status_code);
        free(res_parsed->header);
        free(res_parsed);
        return NULL;
    }
    /* End Status Code*/
    


    /* Location */
    const char *location_start = strstr(res_parsed->header, "Location: "); 
    if (check_status_code(status_code) == 1) {
        printf("Status code: %d --- Redirection\n", status_code);
        if (location_start) {
            location_start += 10;
            const char *location_end = strstr(location_start, "\r\n");
            res_parsed->type = RESPONSE_TYPE_REDIRECTION;
            res_parsed->Content.redirection_data.location = strndup(location_start, (size_t)(location_end - location_start));
            printf("New target: %s\n", res_parsed->Content.redirection_data.location);
            return res_parsed; /* Get back to redo everything */
        }
        printf("No location tag was found.\n");
        return NULL;
    }
    /* End Location */

    printf("Status code: %d --- OK\n", status_code); /* Only case left */

    /* Content Type */
    const char *content_type = open_mind_strstr(res_parsed->header, "Content-Type: ");
    if (!content_type) {
        printf("Error in parsing Content type.\n");
        return NULL;
    }
    content_type += 14;
    if (strncmp(content_type, "image", 5) == 0) {
        if (!flag) {
            free(res_parsed->header);
            free(res_parsed);
            fprintf(stderr, "Image Links are not supported.\n");
            return NULL;
        }
        /* Check Image Type */
        content_type += 6;
        const char *type_end = strstr(content_type, "\r\n");
        char *img_type = strndup(content_type, (size_t)(type_end - content_type));
        if (check_type_img(img_type)) {
            res_parsed->type = RESPONSE_TYPE_IMAGE;
            res_parsed->Content.image_data.img_type = img_type;
            return res_parsed;
        } else if (!check_type_img(img_type)) {
            res_parsed->type = RESPONSE_TYPE_IMAGE;
            res_parsed->Content.image_data.img_type = NULL;
            return res_parsed;
        }
        /* End */
    }
    /* End */

    // /* Transfert Encoding Method */
    const char  *encoding_type = strstr(res_parsed->header, "Transfer-Encoding: ");
    if (!encoding_type) {
        printf("Error in parsing Encoding type.\n");
        return NULL;
    }
    encoding_type += 19;
    if (strncmp(encoding_type, "chunked", 7) == 0) {
        res_parsed->type = RESPONSE_TYPE_GENERIC;
        res_parsed->Content.generic_data.is_chunked = true;
    } 
    /* End */


    /* Body */
    const unsigned char *body_start = strstr(raw_response, "\r\n\r\n"); /* Where the Header ends */
    if (!body_start) {
        printf("Error in parsing Body.\n");
        return NULL;     
    }
    body_start += 4;
    res_parsed->type = RESPONSE_TYPE_GENERIC;
    res_parsed->Content.generic_data.body = strdup(body_start);
    // printf("\n\n\n\n%01x\n\n\n\n", res_parsed->body);
    // free(response);
    /* End */

    return res_parsed;
}


void decode_body(char *body) {
    char *encoded_body = body; 
    char *temp_body = malloc(PAGE_SIZE * sizeof(char)); /* Temporary buffer for decoded body */
    if (!temp_body) {
        perror("Malloc: Failed allocating Chunked body");
        return;
    }

    size_t total_size = 0;

    while (1) {
        char *chunk_end;
        long int chunk_size = strtol(encoded_body, &chunk_end, 16);

        if (chunk_size == 0) {
            break; /* End of the body -> size 0 */
        }

        encoded_body = chunk_end + 2; /* Skip the size + \r\n */

        char *new_temp_body = realloc(temp_body, total_size + chunk_size + 1);
        if (!new_temp_body) {
            free(temp_body);
            perror("Realloc: Failed reallocating Chunked body");
            return;
        }

        temp_body = new_temp_body;
        memcpy(temp_body + total_size, encoded_body, chunk_size);
        total_size += chunk_size;
        encoded_body += chunk_size + 2; /* Skip the chunk + \r\n */
    }

    temp_body[total_size] = '\0'; 

    memcpy(body, temp_body, total_size + 1); 
    free(temp_body); 
}
