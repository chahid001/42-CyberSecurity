#include "spider.h"


void    parse_html(const char *html_source) {

    const char *img_tag = "<img ";
    const char *src_attribute = "src=\"";

    // while(html_source) {
        char* pos = strstr(html_source, img_tag);
        char* src_start = strstr(pos, src_attribute);
        src_start += strlen(src_attribute); // pass src="
        char* src_end = strchr(src_start, '"');
        size_t len = (size_t)(src_end - src_start);
        char *img_path = malloc((len * sizeof(char)) + 1);
        memcpy(img_path, src_start, len);
        img_path += '\0';
        printf("srcsss: %s\n", img_path);
        printf("/////////////\n");

        printf("size: %d\n", strlen(src_start));
        printf("size: %d\n", strlen(src_end));
    //     html_source++;
    // }

}