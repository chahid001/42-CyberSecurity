#include "spider.h"



int main(int argc, char** argv) {

    char* url = ft_args(argc, argv);

    printf("URL: %s\n", url);
    parse_url(url);
    // int f = create_socket(get_ipv4(url));
    // char *html = ft_http(f, url);
    // parse_html(html);
    // system("leaks spider");
}