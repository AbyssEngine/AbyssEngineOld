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
    thread * result = malloc(sizeof(thread));
    result->thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_func, arg, 0, &result->thread_id);
    result->cancelled = FALSE;
    return result;
}

void thread_join(thread *source) {
    WaitForSingleObject(source->thread, INFINITE);
    CloseHandle(source->thread);
    free(source);
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

void mutex_lock(mutex *source) {
    WaitForSingleObject(source->mutex, INFINITE);
}

void mutex_unlock(mutex *source) {
    ReleaseMutex(source->mutex);
}
