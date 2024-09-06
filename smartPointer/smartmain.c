#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdarg.h>
#include <error.h>

#define MAX_STRING_SIZE 100

// 스마트 포인터 관련 정의
typedef struct SmartPtr {
    void *ptr;
    int *ref_count;
    pthread_mutex_t *mutex;
} SmartPtr;

#define CREATE_SMART_PTR(type, ...) create_smart_ptr(sizeof(type), __VA_ARGS__)

// 네트워크 정보 구조체
typedef struct {
    char ip[INET_ADDRSTRLEN];  // IPv4 주소를 저장
    sa_family_t family;        // 주소 패밀리 (AF_INET 등)
} NetworkInfo;

// 네트워크 정보를 얻는 함수
NetworkInfo get_local_network_info() {
    struct addrinfo hints, *res;
    NetworkInfo net_info;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;  // IPv4
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;  // 로컬 호스트를 위한 IP 주소 찾기

    // 호스트 이름을 통해 네트워크 정보 얻기
    char hostname[256];
    gethostname(hostname, sizeof(hostname));
    if (getaddrinfo(hostname, NULL, &hints, &res) != 0) {
        perror("getaddrinfo 실패");
        exit(EXIT_FAILURE);
    }

    // 네트워크 정보 저장 (IPv4 주소)
    struct sockaddr_in *ipv4 = (struct sockaddr_in *)res->ai_addr;
    inet_ntop(AF_INET, &(ipv4->sin_addr), net_info.ip, INET_ADDRSTRLEN);
    net_info.family = res->ai_family;

    freeaddrinfo(res);  // 동적 할당된 메모리 해제
    return net_info;
}

// 스마트 포인터 생성 함수 (가변 인자 사용)
SmartPtr create_smart_ptr(size_t size, ...) {
    SmartPtr sp;
    sp.ptr = malloc(size);
    sp.ref_count = (int *)malloc(sizeof(int));
    *(sp.ref_count) = 1;
    sp.mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(sp.mutex, NULL);

    // 가변 인자를 통해 초기화
    va_list args;
    va_start(args, size);

    if (size == sizeof(int)) {
        int value = va_arg(args, int);
        *(int *)sp.ptr = value;
    } else if (size == sizeof(char) * MAX_STRING_SIZE) {
        const char *str = va_arg(args, const char *);
        strncpy((char *)sp.ptr, str, MAX_STRING_SIZE);
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

// 스레드에서 사용할 함수 예시
void* thread_function(void* arg) {
    int thread_num = *((int*)arg);

    // 네트워크 정보 가져오기
    NetworkInfo net_info = get_local_network_info();

    // 스레드 시작 메시지
    printf("Thread %d: 시작 - 로컬 IP 주소: %s\n", thread_num, net_info.ip);

    sleep(1); // 작업을 모방하기 위한 대기 시간

    // 스레드 종료 메시지
    printf("Thread %d: 종료 - 주소 패밀리: %d\n", thread_num, net_info.family);
    return NULL;
}

// 메인 함수 - 스마트 포인터 사용 예시
int main() {
    int num_threads;
    int sum = 0;
    char strsum[1024] = "";

    // 사용자로부터 생성할 스레드 개수를 입력받습니다.
    printf("생성할 스레드의 수를 입력하세요: ");
    scanf("%d", &num_threads);

    // 스레드 포인터와 스레드 번호 동적 할당
    pthread_t *threads = (pthread_t *)malloc(sizeof(pthread_t) * num_threads);
    int *thread_ids = (int *)malloc(sizeof(int) * num_threads);

    // 각 스레드에서 사용할 int와 char 값을 저장할 배열 동적 할당
    int *int_num_values = (int *)malloc(sizeof(int) * num_threads);
    char (*char_ptr1_values)[MAX_STRING_SIZE] = (char (*)[MAX_STRING_SIZE])malloc(sizeof(char[MAX_STRING_SIZE]) * num_threads);

    if (threads == NULL || thread_ids == NULL || int_num_values == NULL || char_ptr1_values == NULL) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }

    // 사용자 정의 스레드 수 만큼 int 값 입력을 받습니다.
    for (int i = 0; i < num_threads; i++) {
        thread_ids[i] = i + 1;

        // 사용자로부터 int_num 입력받기
        printf("스레드 %d에서 사용할 int_num 값을 입력하세요: ", thread_ids[i]);
        scanf("%d", &int_num_values[i]);
    }

    // 사용자 정의 스레드 수 만큼 char 값을 입력받습니다.
    for (int i = 0; i < num_threads; i++) {
        thread_ids[i] = i + 1;

        // 사용자로부터 char_ptr1 입력받기
        printf("스레드 %d에서 사용할 문자열을 입력하세요: ", thread_ids[i]);
        scanf(" %[^\n]%*c", char_ptr1_values[i]);
    }

    // 사용자 정의 스레드 수 만큼 스레드를 생성
    for (int i = 0; i < num_threads; i++) {
        // 스레드 생성
        pthread_create(&threads[i], NULL, thread_function, &thread_ids[i]);

        // 각 스레드에서 사용할 int 스마트 포인터 생성
        SmartPtr int_num = CREATE_SMART_PTR(int, int_num_values[i]);  // 사용자 입력값으로 초기화

        // 참조 카운트 증가
        retain(&int_num);

        // 스레드별로 할당된 값을 출력
        printf("스레드 %d: int_num = %d\n", thread_ids[i], *(int *)int_num.ptr);
        sum += *(int *)int_num.ptr;
        
        // 스마트 포인터 해제 (참조 카운트 감소)
        release(&int_num);
    }
    printf("결과 : %d\n", sum);

    // 모든 스레드가 종료될 때까지 대기
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);

        // 각 스레드가 종료된 후 사용할 char 스마트 포인터 생성
        SmartPtr char_ptr1 = CREATE_SMART_PTR(char[MAX_STRING_SIZE], char_ptr1_values[i]);

        // 참조 카운트 증가
        retain(&char_ptr1);

        // 스레드 종료 메시지 출력
        printf("스레드 %d: %s\n", thread_ids[i], (char *)char_ptr1.ptr);
        strcat(strsum, (char *)char_ptr1.ptr);

        // 스마트 포인터 해제 (참조 카운트 감소)
        release(&char_ptr1);
    }
    printf("결과 : %s\n", strsum);

    // 동적으로 할당된 스레드 포인터와 스레드 ID 배열 해제
    if (threads != NULL) {
        free(threads);
        threads = NULL;
        printf("스레드 포인터 메모리 해제 완료\n");
    }

    if (thread_ids != NULL) {
        free(thread_ids);
        thread_ids = NULL;
        printf("스레드 ID 배열 메모리 해제 완료\n");
    }

    if (int_num_values != NULL) {
        free(int_num_values);
        int_num_values = NULL;
        printf("int_num_values 배열 메모리 해제 완료\n");
    }

    if (char_ptr1_values != NULL) {
        free(char_ptr1_values);
        char_ptr1_values = NULL;
        printf("char_ptr1_values 배열 메모리 해제 완료\n");
    }


    return 0;
}
