#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: print_file_std <path>\n");
        return 1;
    }

    for (int i = 1; i < argc; ++i) {
        FILE *f = fopen(argv[i], "rb");
        if (!f) {
            fprintf(stderr, "open failed: %s\n", strerror(errno));
            return 1;
        }

        unsigned char buf[1024];
        size_t n;
        while ((n = fread(buf, 1, sizeof(buf), f)) > 0) {
            for (size_t i = 0; i < n; ++i) {
                buf[i] = toupper(buf[i]);
            }

            if (fwrite(buf, 1, n, stdout) != n) {
                fprintf(stderr, "write failed\n");
                fclose(f);
                return 1;
            }
        }

        if (ferror(f)) {
            fprintf(stderr, "read failed\n");
            fclose(f);
            return 1;
        }

        fclose(f);
    }
    return 0;
}
