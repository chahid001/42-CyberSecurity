#include "spider.h"


void    parse_html(const char *html_source) {

    const char *img_tag = "<img ";
    const char *src_attribute = "src=\"";

    char* pos = html_source;
    // while(html_source) {
    while ((pos = strstr(pos, img_tag)) != NULL) {

        char* src_start = strstr(pos, src_attribute);
        if (src_start) {
            src_start += strlen(src_attribute); // pass src="
            char* src_end = strchr(src_start, '"');

            if (src_end) {
                size_t len = (size_t)(src_end - src_start);
                char *img_path = malloc((len * sizeof(char)) + 1);
                memcpy(img_path, src_start, len);
                img_path += '\0';
                printf("srcsss: %s\n", img_path);
                free(img_path);
            }
        }
        pos++;
    }
        
        
        
        
        
        
        
        // printf("srcsss: %s\n", img_path);
        // printf("/////////////\n");

        // printf("size: %d\n", strlen(src_start));
        // printf("size: %d\n", strlen(src_end));
    //     html_source++;
    // }

}