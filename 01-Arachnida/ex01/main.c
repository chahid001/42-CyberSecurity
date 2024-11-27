#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

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
                if (optarg && isdigit(optarg)) {
                    level = atoi(optarg);
                } else {
                    fprintf(stderr, "Error: -l Level Depth should be a numeric value.\n");
                    return 1;
                }
                break;
            case 'p':
                path = optarg;
                break;
            default:
                fprintf(stderr, "Usage ./spider -r [-l level][-p path] URL");
                return 1;
        }
    }

    if (!recursive) {
        fprintf(stderr, "Usage ./spider -r [-l level][-p path] URL");
        return 1;
    }
    if (optind < argc) { 
        url = argv[optind];
    }





}