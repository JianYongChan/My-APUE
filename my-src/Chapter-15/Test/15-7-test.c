#include "apue.h"

void echo_str(char *str) {
    for (int i = 0; i < strlen(str); ++i) {
        if (write(STDOUT_FILENO, &str[i], 1) != 1)
            err_sys("write error");
    }
}

int main(void) {
    pid_t pid;

    TELL_WAIT();
    if ((pid = fork()) < 0) {
        err_sys("fork error");
    } else if (pid > 0) {
        echo_str("This is Parent");
        // TELL_CHILD(pid);
        if (waitpid(pid, NULL, 0) < 0)
            err_sys("waitpid error");
    } else {
        // WAIT_PARENT();
        echo_str("This is Child");
    }
}
