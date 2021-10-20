/**
 * Copyright (C) 2021 Tim Sarbin
 * This file is part of AbyssEngine <https://github.com/AbyssEngine>.
 *
 * AbyssEngine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * AbyssEngine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with AbyssEngine.  If not, see <http://www.gnu.org/licenses/>.
 */

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

bool thread_same(thread *other) { return pthread_equal(pthread_self(), other->pthread) != 0; }

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

thread *thread_get_current() {
    thread *result = malloc(sizeof(thread));
    result->pthread = pthread_self();
    return result;
}
