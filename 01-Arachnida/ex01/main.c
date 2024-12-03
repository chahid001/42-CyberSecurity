#include "spider.h"



int main(int argc, char** argv) {

    t_opts *opts = ft_args(argc, argv);

    char *response = ft_request(opts->url);

    printf("%s", response);
    free(opts);
    free(response);
    system("leaks spider");
}