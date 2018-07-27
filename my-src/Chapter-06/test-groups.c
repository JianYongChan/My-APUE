#include "apue.h"

// 为什么我以root身份运行此程序会成功
// 而以普通用户身份(ctom)运行则报`Invalid arguments`错误
int main() {
    gid_t grouplist[7];
    int grp_num;

    if ((grp_num = getgroups(7, grouplist)) >= 0) {
        printf("group number: ");
        for (int i = 0; i < grp_num; ++i) {
            printf("%d ", grouplist[i]);
        }
        printf("\n");
    } else {
        err_sys("getgroups error");
    }
}