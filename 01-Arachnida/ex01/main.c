#include "spider.h"

static void print_usage() {
    fprintf(stderr, "Usage ./spider -r [-l level][-p path] URL\n");
    exit(1);
}

int main(int argc, char** argv) {

    int opt;
    int recursive = 0;
    int level = -1;

    char *path = NULL;
    char *url = NULL;

    while((opt = getopt(argc, argv, "-rl:p:")) != -1) {
        switch (opt) {
            case 'r':
                recursive = 1;
                break;
            case 'l':
                if (optarg && isdigit(optarg[0])) {
                    level = atoi(optarg);
                } else {
                    fprintf(stderr, "Error: -l Level Depth should be a numeric value.\n");
                    print_usage();
                }
                break;
            case 'p':
                path = optarg;
                break;
            default:
                print_usage();
        }
    }

    if (!recursive) {
        print_usage();
    }
    if (optind < argc) { 
        url = argv[optind++]; // take value then increment
        printf("URL: %s\n", url);
    } else {
        fprintf(stderr, "Please provide a URL.\n");
    }

    if (optind < argc) {
        print_usage();
    }

    if (level > 0) {
        printf("level: %d\n", level);
    }
    if (path) {
        printf("path: %s\n", path);
    }

    int f = create_socket(get_ipv4(url));
    char *html = ft_http(f, url);
    parse_html(html);
    system("leaks spider");
}