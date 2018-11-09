#include <stdio.h>
#include <stdlib.h>

int is_little_endian() {
    short two_bytes = 0x0737;
    char *p = (char*)&two_bytes;

    if (p[0] == 0x37)
        return 1;   // little_endian的最低有效字节(LSB)包含最小字节地址
    else 
        return 0;
}

int main() {
    if (is_little_endian())
        printf("little endian");
    else
        printf("big endian");

    return 0;
}
