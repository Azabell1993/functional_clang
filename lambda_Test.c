#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 람다 함수 매크로
#define lambda(return_type, function_body) ({ \
    return_type __fn__ function_body \
    __fn__; \
})

// Example 구조체 정의
typedef struct {
    int (*hello_print)(int);
    char* (*print_example)(char*);
} Example;

// hello_print 함수 정의
int hello_print(int num) {
    // 숫자를 문자열로 변환
    char result[10];
    sprintf(result, "%d", num);
    return atoi(result);
}

// print_example 함수 정의
char* print_example(char* myname, int num) {
    // 결과를 담을 문자열 버퍼 할당
    char result[100];
    sprintf(result, "My name is %s and my birthday is on the %dth day!", myname, num);
    // 문자열을 복사하여 반환
    return strdup(result);
}

// main 함수
int main() {
    // Example 구조체 메모리 할당
    Example* example = malloc(sizeof(Example));
    if (!example) {
        perror("Failed to allocate memory");
        return 1; // 메모리 할당 실패 시 프로그램 종료
    }

    // hello_print 함수 할당
    example->hello_print = hello_print;
    // print_example 함수 할당
    example->print_example = lambda(char*, (char* myname) {
        return print_example(myname, example->hello_print(19930315));
    });

    // 결과 출력
    char* result = example->print_example("Park Ji Woo");
    printf("%s\n", result);

    // 동적 메모리 해제
    free(result);
    free(example);

    return 0;
}
