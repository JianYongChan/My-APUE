#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define handle_error(msg)   \
    do {                    \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while(0)

int main(int argc, char *argv[]) {
    FILE *out, *in;
    int v, s;
    size_t size;
    char *ptr;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s '<num>...'\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // argv[1]是一连串数字，空格分隔
    // 以argv[1]作为内存流缓冲区
    in = fmemopen(argv[1], strlen(argv[1]), "r");
    if (in == NULL)
        handle_error("fmemopen");

    out = open_memstream(&ptr, &size);
    if (out == NULL)
        handle_error("open_memstream");

    // 从in中读取int，将int的平方写入out
    for (;;) {
        s = fscanf(in, "%d", &v);
        if (s <= 0)
            break;

        s = fprintf(out, "%d ", v * v);
        if (s == -1)
            handle_error("fprintf");
    }

    fclose(in);
    fclose(out);

    printf("size = %zu; ptr = %s\n", size, ptr);

    // open_memstream创建的流需要自行释放缓冲区
    free(ptr);
    exit(EXIT_SUCCESS);
}