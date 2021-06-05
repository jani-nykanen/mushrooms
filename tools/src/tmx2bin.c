#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>

#include "types.h"


static char* buffer = NULL;


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
    i32 i;

    if (argc <= 2) {
        
        fprintf(stderr, "At least two arguments must be passed!\nHelp: tmx2bin output.bin file1 <file2> ... <fileN>\n");
        return 1;
    }

    output = argv[1];
    for (i = 2; i < argc; ++ i) {

        if (load_file_to_buffer(argv[i]) != 0) {

            return 1;
        }

        printf("%s\n", buffer);
    }

    return 0;
}
