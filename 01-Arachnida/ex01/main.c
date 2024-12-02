#include "spider.h"



int main(int argc, char** argv) {

    t_opts *opts = ft_args(argc, argv);

    printf("Level: %d\n", opts->level);
    printf("Path: %s\n", opts->path);

    // parse_url(url);
    // int f = create_socket(get_ipv4(url));
    // char *html = ft_http(f, url);
    // parse_html(html);
    // system("leaks spider");
}