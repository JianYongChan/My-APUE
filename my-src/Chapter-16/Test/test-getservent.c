#include "apue.h"
#include <netdb.h>


static void pr_servent(const struct servent *sp) {
    if (sp == NULL)
        return;
    printf("name: %s\n", sp->s_name);
    printf("port: %d\n", sp->s_port);
    printf("proto: %s\n", sp->s_proto);
}

int
main(void) {
    // 打印前100条entry
    struct servent *sp;
    int cnt = 100;

    while ((sp = getservent()) != NULL && cnt-- != 0) {
        pr_servent(sp);
        printf("\n");
    }
}
