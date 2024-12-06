#include "spider.h"



int main(int argc, char** argv) {

    t_Opts *opts = ft_args(argc, argv);

    ft_network(opts->url);


    // parse_html(response);
     // -> Mandatory
    free(opts->url->uri);
    free(opts->url->host);
    free(opts->url);
    free(opts);
    // free(response);
    system("leaks spider");
}