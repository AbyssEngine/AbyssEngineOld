#ifndef LIBABYSS_THREADING_H
#define LIBABYSS_THREADING_H

typedef struct thread thread;
typedef struct mutex mutex;

thread *thread_create(void *(*thread_func)(void *arg), void *arg);
void thread_join(thread *source);
void thread_cancel(thread *source);
void thread_cancel_checkpoint();

#ifdef _WIN32

#else
#include <pthread.h>
#define thread_cleanup_push(func, val) pthread_cleanup_push(func, val)
#define thread_cleanup_pop(execute) pthread_cleanup_pop(execute)
#endif // _WIN32

mutex *mutex_create();
void mutex_destroy(mutex *source);
void mutex_lock(mutex *mutex);
void mutex_unlock(mutex *mutex);

#endif // LIBABYSS_THREADING_H
