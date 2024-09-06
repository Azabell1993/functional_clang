#include <dlfcn.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdarg.h>
#include <stdbool.h>
#include <errno.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <fcntl.h>

// 고급 오류 처리 함수 구현
#include "ename.c.inc"

#define BUF_SIZE 100
#define NUM_THREADS 3
#define MAX_STRING_SIZE 100

typedef struct SmartPtr SmartPtr;
#define CREATE_SMART_PTR(type, ...) create_smart_ptr(sizeof(type), __VA_ARGS__)

static void retain(SmartPtr *sp);
static void release(SmartPtr *sp);
static void safe_kernel_printf(const char *format, ...);
static void kernel_errExit(const char *format, ...);
static void outputError(bool useErr, int err, bool flushStdout, const char *format, va_list ap);
static void kernel_socket_communication(int sock_fd, const char *message, char *response, size_t response_size);
static void kernel_create_thread(pthread_t *thread, void *(*start_routine)(void *), void *arg);
static void* thread_function(void* arg);
static void terminate(bool useExit3);
static void kernel_join_thread(pthread_t thread);
static void kernel_wait_for_process(pid_t pid);

static pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct SmartPtr {
    void *ptr;
    int *ref_count;
    pthread_mutex_t *mutex;
} SmartPtr;

// 스마트 포인터 초기화 함수 (가변 인자 사용)
SmartPtr create_smart_ptr(size_t size, ...) {
    SmartPtr sp;
    sp.ptr = malloc(size);
    if (sp.ptr == NULL) {
        perror("Failed to allocate memory for smart pointer");
        exit(EXIT_FAILURE);
    }

    sp.ref_count = (int *)malloc(sizeof(int));
    if (sp.ref_count == NULL) {
        perror("Failed to allocate memory for ref_count");
        free(sp.ptr);
        exit(EXIT_FAILURE);
    }
    *(sp.ref_count) = 1;

    sp.mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    if (sp.mutex == NULL) {
        perror("Failed to allocate memory for mutex");
        free(sp.ptr);
        free(sp.ref_count);
        exit(EXIT_FAILURE);
    }
    pthread_mutex_init(sp.mutex, NULL);

    // 가변 인자를 사용하여 초기화
    va_list args;
    va_start(args, size);

    if (size == sizeof(int)) {
        int value = va_arg(args, int);
        *(int *)sp.ptr = value;
    } else if (size == sizeof(float)) {
        float value = (float)va_arg(args, double);
        *(float *)sp.ptr = value;
    } else if (size == sizeof(char) * MAX_STRING_SIZE) {
        const char *str = va_arg(args, const char *);
        strncpy((char *)sp.ptr, str, MAX_STRING_SIZE);  // 문자열 초기화
    }

    va_end(args);
    return sp;
}

// 참조 카운트 증가
void retain(SmartPtr *sp) {
    pthread_mutex_lock(sp->mutex);
    (*(sp->ref_count))++;
    pthread_mutex_unlock(sp->mutex);
}

// 참조 카운트 감소 및 메모리 해제
void release(SmartPtr *sp) {
    int should_free = 0;

    pthread_mutex_lock(sp->mutex);
    (*(sp->ref_count))--;
    if (*(sp->ref_count) == 0) {
        should_free = 1;
    }
    pthread_mutex_unlock(sp->mutex);

    if (should_free) {
        free(sp->ptr);
        free(sp->ref_count);
        pthread_mutex_destroy(sp->mutex);
        free(sp->mutex);
    }
}

// 안전한 출력 함수
static void safe_kernel_printf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    pthread_mutex_lock(&print_mutex);  // 출력 뮤텍스 잠금
    vprintf(format, args);  // 커널 printf 함수 호출
    pthread_mutex_unlock(&print_mutex);  // 출력 뮤텍스 해제

    va_end(args);
}

// 오류 출력 함수
static void kernel_errExit(const char *format, ...) {
    va_list argList;

    va_start(argList, format);
    outputError(true, errno, true, format, argList);
    va_end(argList);

    terminate(true);  // 프로그램 종료
}

static void outputError(bool useErr, int err, bool flushStdout, const char *format, va_list ap) {
    char buf[BUF_SIZE], userMsg[BUF_SIZE], errText[BUF_SIZE];

    vsnprintf(userMsg, BUF_SIZE, format, ap);

    if (useErr)
        snprintf(errText, BUF_SIZE, " [%s %s]",
                 (err > 0 && err <= MAX_ENAME) ? ename[err] : "?UNKNOWN?", strerror(err));
    else
        snprintf(errText, BUF_SIZE, ":");

    snprintf(buf, BUF_SIZE, "ERROR%s %s\n", errText, userMsg);

    if (flushStdout)
        fflush(stdout);

    fputs(buf, stderr);
    fflush(stderr);
}

// 종료 처리
static void terminate(bool useExit3) {
    char *s = getenv("EF_DUMPCORE");

    if (s != NULL && *s != '\0')
        abort();
    else if (useExit3)
        exit(EXIT_FAILURE);
    else
        _exit(EXIT_FAILURE);
}

// 스레드 함수 예제
static void* thread_function(void* arg) {
    int thread_num = *((int*)arg);
    safe_kernel_printf("Thread %d: 시작\n", thread_num);
    sleep(1); // 작업을 모방하기 위한 대기 시간
    safe_kernel_printf("Thread %d: 종료\n", thread_num);
    return NULL;
}

// 소켓 통신 함수
static void kernel_socket_communication(int sock_fd, const char *message, char *response, size_t response_size) {
    if (write(sock_fd, message, strlen(message)) == -1) {
        printf("Failed to send message through socket");
    }

    ssize_t bytes_read = read(sock_fd, response, response_size - 1);
    if (bytes_read == -1) {
        printf("Failed to receive message from socket");
    }
    response[bytes_read] = '\0';
}

// 자식 프로세스 대기 함수
static void kernel_wait_for_process(pid_t pid) {
    int status;
    if (waitpid(pid, &status, 0) < 0) {
        printf("Failed to wait for process");
    }
}

// 스레드 생성 함수
static void kernel_create_thread(pthread_t *thread, void *(*start_routine)(void *), void *arg) {
    int err = pthread_create(thread, NULL, start_routine, arg);
    if (err != 0) {
        printf("Failed to create thread");
    }
}

// 스레드 조인 함수
static void kernel_join_thread(pthread_t thread) {
    int err = pthread_join(thread, NULL);
    if (err != 0) {
        printf("Failed to join thread");
    }
}
