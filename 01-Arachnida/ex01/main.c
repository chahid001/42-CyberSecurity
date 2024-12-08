#include "spider.h"

static free_them_all(t_Opts *opts) {
    
    free(opts->url->uri);
    free(opts->url->host);
    free(opts->url);
}

int main(int argc, char** argv) {

    char *response;
    t_Opts *opts = ft_args(argc, argv);
    t_HTTP_Response *parsed_response;

    response = ft_network(opts->url);

    if (!response) {
        free_them_all(opts);
        free(opts);
        exit(EXIT_FAILURE);
    } 

    parsed_response = parse_http_response(response);

    if (!parsed_response) {
        free(response);
        free_them_all(opts);       
    }

    if (parsed_response->location != NULL) {
        free(response);
        free_them_all(opts);
        opts->url = parse_url(parsed_response->location);
        response = ft_network(opts->url);
        free(parsed_response->location);
        free(parsed_response->header);
        free(parsed_response);
        parsed_response = parse_http_response(response);
    }

    if (parsed_response->is_chunked == true) {
        parse_html(parsed_response->body);
    } else {

    }

    free(response);
    free_them_all(opts);
    free(opts);

    system("leaks spider");
}