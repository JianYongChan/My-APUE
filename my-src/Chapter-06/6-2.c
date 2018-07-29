#include "apue.h"
#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <stddef.h>
#include <string.h>

struct passwd* mygetpwnam(const char *name) {
    struct passwd *ptr;

    setpwent();
    while((ptr = getpwent()) != NULL) {
        if (strcmp(name, ptr->pw_name) == 0) {
            break;
        }
    }
    endpwent();
    
    return(ptr);
}

int main(int argc, char *argv[]) {
    struct passwd *ptr;

    if (argc != 2) {
        err_quit("usage: %s <name>", argv[0]);
    }
    if((ptr = mygetpwnam(argv[1])) != NULL) {
        printf("name: %s\n", ptr->pw_name);
        printf("passwd: %s\n", ptr->pw_passwd);
        printf("uid: %d\n", ptr->pw_uid);
        printf("gid: %d\n", ptr->pw_gid);
        printf("comment: %s\n", ptr->pw_gecos);
        printf("home: %s\n", ptr->pw_dir);
        printf("shell: %s\n", ptr->pw_shell);
    }
}
