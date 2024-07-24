#include "ASMTest.h"

PROC(main)
    // 정수 출력
    PUSH(42);        // 스택에 정수 42를 푸시
    CALL(print_int); // print_int 함수를 호출하여 정수를 출력
    POP(n);          // 스택에서 값을 팝하여 n에 저장

    // 개행 추가
    const char* newline = "\n";
    PUSH((INT)newline);
    CALL(print_str);
    POP(n);

    // 문자열 출력
    const char* str = "Hello, World!";
    PUSH((INT)str);  // 스택에 문자열의 주소를 푸시
    CALL(print_str); // print_str 함수를 호출하여 문자열을 출력
    POP(n);          // 스택에서 값을 팝하여 n에 저장

    // 개행 추가
    PUSH((INT)newline);
    CALL(print_str);
    POP(n);
RETURN();        // main 함수 종료
ENDP
