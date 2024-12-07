#include "spider.h"

/*
    Parsing Args using getops, the behavior can change from Mac to Linux.
    Parsing URL, LEVEL & PATH.

    Malloc: opts object
*/

static void print_usage() {
    fprintf(stderr, "Usage ./spider -r [-l level][-p path] URL\n");
    exit(1);
}

t_Opts   *ft_args(int argc, char **argv) {

    t_Opts *opts;

    int opt;
    int recursive = 0;
    int level = -1;

    char *path = "./data"; /* Default path */
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

     /* 
        We take the value of the URL, then we increment **optind**
        If optind still smaller than argc it mean we have undesired arguments -> error.
     */

    if (optind < argc) { 
        url = argv[optind++];
    } else {
        fprintf(stderr, "Please provide a URL.\n");
    } if (optind < argc) {
        print_usage();
    }


    opts = (t_Opts*)malloc(sizeof(t_Opts));

    if (!opts) {
        write(2, "Malloc: Failed to allocate Options object.\n", 43);
        return NULL;
    }

    opts->url = parse_url(url, true);
    opts->path = path;
    if (level > 0) {
        opts->level = level;
    } else {
        opts->level = 5; /* Default value */
    }

    return opts;
} 
  






