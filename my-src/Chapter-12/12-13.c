#include <limits.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>

#define MAXSTRINGSZ 4096

pthread_key_t key;
pthread_once_t init_done = PTHREAD_ONCE_INIT;
pthread_mutex_t env_mutex = PTHREAD_MUTEX_INITIALIZER;

extern char **environ;

static void
thr_init(void) {
    pthread_key_create(&key, free);
}

char *
getenv_3(const char *name) {
    int i, len;
    char *env_buf;

    len = strlen(name);
    pthread_once(&init_done, thr_init);
    pthread_mutex_lock(&env_mutex);

    env_buf = pthread_getspecific(key);
    if (env_buf == NULL) {
        env_buf = (char*)malloc(MAXSTRINGSZ);
        if (env_buf == NULL) {
            pthread_mutex_unlock(&env_mutex);
            return NULL;
        }
    }

    for (i = 0; environ[i] != NULL; ++i) {
        if ((strncmp(name, environ[i], len) == 0) && 
                environ[i][len] == '=') {
            strncpy(env_buf, &environ[i][len+1], MAXSTRINGSZ-1);
            pthread_mutex_unlock(&env_mutex);
            return env_buf;
        }
    }
    pthread_mutex_unlock(&env_mutex);
    return NULL;
}
