#include <libabyss/threading.h>
#include <pthread.h>
#include <stdlib.h>

typedef struct thread {
    pthread_t pthread;
} thread;

typedef struct mutex {
    pthread_mutex_t lock;
} mutex;

thread *thread_create(void *(*thread_func)(void *arg), void *arg) {
    thread *result = malloc(sizeof(thread));
    int ret = pthread_create(&result->pthread, NULL, thread_func, arg);

    if (ret) {
        free(result);
        return NULL;
    }

    return result;
}

void thread_join(thread *source) {
    pthread_join(source->pthread, NULL);
    free(source);
}

void thread_cancel(thread *source) { pthread_cancel(source->pthread); }

void thread_cancel_checkpoint() { pthread_testcancel(); }

mutex *mutex_create() {
    mutex *result = malloc(sizeof(mutex));
    if (pthread_mutex_init(&result->lock, NULL)) {
        free(result);
        return NULL;
    }

    return result;
}

void mutex_destroy(mutex *source) {
    pthread_mutex_destroy(&source->lock);
    free(source);
}

void mutex_lock(mutex *mutex) { pthread_mutex_lock(&mutex->lock); }

void mutex_unlock(mutex *mutex) { pthread_mutex_unlock(&mutex->lock); }
