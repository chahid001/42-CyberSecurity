#include "spider.h"



int main(int argc, char** argv) {

    char *response;
    t_Opts *opts = ft_args(argc, argv);

    response = ft_network(opts->url);

    if (!response) {
        free(opts->url->uri);
        free(opts->url->host);
        free(opts->url);
        free(opts);
        exit(EXIT_FAILURE);
    }

    // printf("%s", response);

    parse_http_response(response);
    free(response);   
     // -> Mandatory
    free(opts->url->uri);
    free(opts->url->host);
    free(opts->url);
    free(opts);

    system("leaks spider");
}