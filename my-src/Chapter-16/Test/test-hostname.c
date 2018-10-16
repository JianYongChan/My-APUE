#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>

int main() {
    struct hostent *hp;

    hp = gethostent();
    printf("hostname: %s\naddrtype: %d\n", hp->h_name, hp->h_addrtype);
}
