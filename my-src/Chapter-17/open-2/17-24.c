#include "17-26.h"

// 验证客户进程发送的参数个数是否正确
// 然后将路径名和打开模式存储在全局变量中
int
cli_args(int argc, char **argv) {
    if (argc != 3 || strcmp(argv[0], CS_OPEN) != 0) {
        /*
        fprintf(stdout, "argc = %d\n", argc);
        for (int i = 0; i < argc; i++) 
            printf("argv[%d] = %s\n", i, argv[i]);
        */
        strcpy(errmsg, "usage: <pathname> <oflag>\n");
        return -1;
    }
    pathname = argv[1];
    oflag = atoi(argv[2]);
    return 0;
}
