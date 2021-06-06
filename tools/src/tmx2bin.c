#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "types.h"


static char* buffer = NULL;


static i32 find_substring(const str keyword) {

    const char* p = strstr(buffer, keyword);
    if (p == NULL) return -1;

    return (i32)(size_t)(p - buffer);
}


#define BUFFER_LEN 64


static i32 get_numeric_param_value(const str key) {

    char cbuffer [64];
    i32 i;
    char c;

    i32 pos = find_substring(key);
    if (pos < 0) return 0;

    pos += strlen(key) + 2;

    i = 0;
    while ((c = buffer[pos + i]) != '\"' && i < BUFFER_LEN-1) {

        cbuffer[i ++] = c;
    }
    cbuffer[i] = '\0';

    return strtol(cbuffer, NULL, 10);
}


static bool parse_layer_data(i32* dbuffer, u32 size) {

    static const str START = "<data encoding=\"csv\">";
    static const str END = "</data>";

    char cbuffer [BUFFER_LEN];

    i32 startPos = find_substring(START) + strlen(START);
    i32 endPos = find_substring(END);

    i32 i, j, k;
    char c;

    if (endPos < 0 || startPos < 0) {

        fprintf(stderr, "Failed to read layer data!\n");
        return true;
    }

    j = 0;
    k = 0;
    for (i = startPos; i < endPos; ++ i) {

        c = buffer[i];
        if (c >= '0' && c <= '9') {

            cbuffer[j ++] = c;
        }
        else if (j > 0 && c == ',') {

            cbuffer[j] = '\0';
            dbuffer[k ++] = (i32)strtol(cbuffer, NULL, 10);
            j = 0;
        }
    }

    if (j > 0) {

        cbuffer[j] = '\0';
        dbuffer[k ++] = (i32)strtol(cbuffer, NULL, 10);
    }

    return false;
}


#undef BUFFER_LEN


static i32 load_file_to_buffer(const str path) {

    u32 size, i;
    FILE* f;
    i32 c;

    if (buffer != NULL) free(buffer);

    f = fopen(path, "r");
    if (f == NULL) {

        fprintf(stderr, "Failed to open a file in %s!\n", path);
        return 1;
    }

    fseek(f, 0, SEEK_END);
    size = ftell(f);
    rewind(f);

    buffer = (u8*) malloc(size);
    if (buffer == NULL) {

        fprintf(stderr, "Memory allocation error!\n");
        return 1;
    }
    i = 0;
    while ((c = fgetc(f)) != EOF) {

        buffer[i ++] = (u8)c;
    }
    
    fclose(f);

    return 0;
}


i32 main(i32 argc, str* argv) {

    str output;
    i32 i, j;
    i32 width, height;
    i32* tileBuffer = NULL;

    if (argc <= 2) {
        
        fprintf(stderr, "At least two arguments must be passed!\nHelp: tmx2bin output.bin file1 <file2> ... <fileN>\n");
        return 1;
    }

    output = argv[1];
    for (i = 2; i < argc; ++ i) {

        if (load_file_to_buffer(argv[i]) != 0) {

            return 1;
        }

        width = get_numeric_param_value("width");
        height = get_numeric_param_value("height");

        tileBuffer = (i32*) calloc(width * height, sizeof(i32));
        if (tileBuffer == NULL) {

            fprintf(stderr, "Memory allocation error!\n");
            return 1;
        }

        if (parse_layer_data(tileBuffer, width*height)) {

            return 1;
        }

        free(tileBuffer);
    }

    return 0;
}
