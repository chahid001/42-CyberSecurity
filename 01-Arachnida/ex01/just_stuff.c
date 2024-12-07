#include "spider.h"

int check_status_code(int status_code) {

    if (status_code == 200) {
        printf("Status code: %d --- OK\n", status_code);
        return 0;
    } else if (status_code == 301 || status_code == 302) {
        printf("Status code: %d --- Redirection\n", status_code);
        return 1;
    } else {
        printf("Status code: %d --- KO\n", status_code);
        return -1;        
    }
}