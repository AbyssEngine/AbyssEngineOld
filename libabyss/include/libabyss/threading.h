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

#ifndef LIBABYSS_THREADING_H
#define LIBABYSS_THREADING_H

#include <stdbool.h>

typedef struct thread thread;
typedef struct mutex mutex;

thread *thread_create(void *(*thread_func)(void *arg), void *arg);
void thread_join(thread *source);
void thread_cancel(thread *source);
void thread_cancel_checkpoint();
bool thread_same(thread *other);
thread *thread_get_current();

#ifdef _WIN32
// TODO: Is there a windows way to do this?
#define thread_cleanup_push(func, val)
#define thread_cleanup_pop(execute)
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
