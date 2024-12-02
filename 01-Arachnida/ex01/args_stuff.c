#include "spider.h"

static void print_usage() {
    fprintf(stderr, "Usage ./spider -r [-l level][-p path] URL\n");
    exit(1);
}

t_opts   *ft_args(int argc, char **argv) {

    t_opts *opts;

    int opt;
    int recursive = 0;
    int level = -1;

    char *path = "./data";
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
    } else {
        fprintf(stderr, "Please provide a URL.\n");
    }

    if (optind < argc) {
        print_usage();
    }


    opts = (t_opts*)malloc(sizeof(t_opts));

    //protection

    opts->url = parse_url(url);
    opts->path = path; // Check the path for later
    if (level > 0) {
        opts->level = level;
    } else {
        opts->level = 5;
    }

    return opts;
} 
  






