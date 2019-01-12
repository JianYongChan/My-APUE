#include "apue.h"

#define NSIG 32

void
print_sigset(FILE *of, const char *prefix, const sigset_t *sigset) {
    int sig, cnt;

    cnt = 0;
    for (sig = 1; sig < NSIG; sig++) {
        if (sigismember(sigset, sig)) {
            cnt++;
            fprintf(of, "%s%d (%s)\n", prefix, sig, strsignal(sig));
        }
    }

    if (cnt == 0)
        fprintf(of, "%s<empty signal set>\n", prefix);
}

int
print_sigmask(FILE *of, const char *msg) {
    sigset_t    mask;

    if (msg != NULL)
        fprintf(of, "%s", msg);

    if (sigprocmask(SIG_BLOCK, NULL, &mask) < 0)
        err_sys("sigprocmask error");

    print_sigset(of, "\t\t", &mask);

    return 0;
}

int
print_pending_sigs(FILE *of, const char *msg) {
    sigset_t    pendingsigs;

    if (msg != NULL)
        fprintf(of, "%s", msg);

    if (sigpending(&pendingsigs) < 0)
        err_sys("get pending signals error");

    print_sigset(of, "\t\t", &pendingsigs);

    return 0;
}
