#include <stdio.h>
#include <stdlib.h>

int f1(int val);

int main(void) {
    int val1 = 37, val2 = 0;

    printf("f1(%d) = %d\n", val1, f1(val1));
    printf("f1(%d) = %d\n", val2, f1(val2));
}

int f1(int val) {
    int num = 0;
    int *ptr = &num;
    if (val == 0) {
        int val;
        val = 5;
        ptr = &val;
    }

    return (*ptr + 1);
}
