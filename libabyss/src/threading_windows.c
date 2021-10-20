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
#include <windows.h>

typedef struct thread {
    HANDLE thread;
    DWORD thread_id;
    BOOL cancelled;
} thread;

typedef struct mutex {
    HANDLE mutex;
} mutex;

thread *thread_create(void *(*thread_func)(void *arg), void *arg) {
    thread *result = malloc(sizeof(thread));
    result->thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_func, arg, 0, &result->thread_id);
    result->cancelled = FALSE;
    return result;
}

void thread_join(thread *source) {
    WaitForSingleObject(source->thread, INFINITE);
    CloseHandle(source->thread);
    free(source);
}

bool thread_same(thread *other) {
    DWORD source_id = GetThreadId(GetCurrentThread());
    return source_id == other->thread_id;
}

thread *thread_get_current() {
    thread *result = malloc(sizeof(thread));
    result->thread = GetCurrentThread();
    result->thread_id = GetThreadId(result->thread);
    result->cancelled = false;

    return result;
}

void thread_cancel(thread *source) {
    // TODO: This is the ~worst~ way to end a thread, and will leak memory.
    TerminateThread(source->thread, 0);
}

void thread_cancel_checkpoint() {
    // I don't know if this is a thing on windows...
}

mutex *mutex_create() {
    mutex *result = malloc(sizeof(mutex));
    result->mutex = CreateMutex(NULL, FALSE, NULL);
    return result;
}

void mutex_destroy(mutex *source) {
    CloseHandle(source->mutex);
    free(source);
}

void mutex_lock(mutex *source) { WaitForSingleObject(source->mutex, INFINITE); }

void mutex_unlock(mutex *source) { ReleaseMutex(source->mutex); }
