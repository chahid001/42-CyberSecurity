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
            
            if (imgs && j < 3) {
                if (!parsed_response->Content.image_data.img_type) {
                    if (flag) {
                        free_them_all(opts, socket, parsed_response, raw_response, false);
                        opts->url = parse_url(imgs[i++]);
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






    // if (parsed_response->location != NULL) {
    //     opts->url = parse_url(parsed_response->location);
    //     raw_response = ft_network(opts->url);
    //     free(parsed_response->location);
    //     free(parsed_response->header);
    //     free(parsed_response);
    //     parsed_response = parse_http_response(raw_response);
    // }

//     if (parsed_response->is_chunked == true) {
//         char *body = decode_body(parsed_response->body);
//         printf("%s", body);
//         parse_html(body);
//         free(body);
//     } else if (parsed_response->is_image == true) {
//         printf("Its an image.");
//     } else {
//         parse_html(parsed_response->body);
//     }

//     free(raw_response);
//     free_them_all(opts);
//     free(opts);

//     system("leaks spider");
// }