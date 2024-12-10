#include "spider.h"

int main(int argc, char** argv) {

    char        *raw_response       = NULL;
    t_Socket    *socket             = NULL;
    t_Response  *parsed_response    = NULL;
    t_Opts      *opts               = ft_args(argc, argv);
    char        **imgs              = NULL;
    int         redirection_count   = 0;
    int         max_redirections    = 2;

    int  i                   = 0;
    int j = 0;
    bool                        flag = false;
    while (redirection_count < max_redirections) {

        /* Connect to target */
        socket = ft_network(opts->url);

        /* Get Response */
        raw_response = get_response(socket, opts->url->port);
        if (!raw_response) {
            free_them_all(opts, NULL, NULL, NULL, NULL);
            exit(EXIT_FAILURE);
        }

        /* Parse Response */
        parsed_response = parse_http_response(raw_response);

        if (!parsed_response) {
        
            free_them_all(opts, NULL, NULL, raw_response, NULL);
            exit(EXIT_FAILURE);      
        
        } else if (parsed_response->type == RESPONSE_TYPE_REDIRECTION) {

            free_them_all(NULL, socket, NULL, raw_response, false);
            opts->url = parse_url(parsed_response->Content.redirection_data.location);
            free_them_all(NULL, NULL, parsed_response, NULL, NULL);
            redirection_count++;
            continue;
        
        } else if (parsed_response->type == RESPONSE_TYPE_GENERIC) {

            if (parsed_response->Content.generic_data.is_chunked) {
                /* Decode Chunked body */
                decode_body(parsed_response->Content.generic_data.body);
            }
            


            imgs = parse_html(parsed_response->Content.generic_data.body);
            flag = true;
            if (imgs) {
                free_them_all(NULL, socket, parsed_response, raw_response, false);
                opts->url = parse_url(imgs[i]);
                continue;
            }  

        } else if (parsed_response->type == RESPONSE_TYPE_IMAGE) {
            
            if (imgs[i] && j < opts->level) {

                if (!parsed_response->Content.image_data.img_type) {
                    if (flag) {
                        free_them_all(opts, socket, parsed_response, raw_response, false);
                        opts->url = parse_url(imgs[i++]);
                        printf("\n\n\n\n %d \n\n\n\n", i);
                        continue;
                    } else {
                        free_them_all(opts, socket, parsed_response, raw_response, true);
                        fprintf(stderr, "File Type not supported.\n");
                        exit(EXIT_FAILURE);
                    } 
                }
                download_stuff(opts->url, parsed_response->Content.image_data.img_type, j+1);
                free_them_all(opts, socket, parsed_response, raw_response, false);
                opts->url = parse_url(imgs[i++]);
                j++;
                continue;
            } else {
                if (j < opts->level) {
                    printf("Available images is smaller than the default Level.\n");
                    exit(EXIT_SUCCESS);
                }
                break;
            }
        }
    }
    
    if (redirection_count >= max_redirections) {
        fprintf(stderr, "Too many redirects.\n");
        free_them_all(opts, NULL, NULL, NULL, true);
        exit(EXIT_FAILURE);
    }
}


