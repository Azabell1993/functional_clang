#include "ASMTEST.h"
#include <time.h>

/*
    HelloWorld 출력
*/
#if DTCASE == 1
PROC(main)
    STRING msg = "Hello, World!";
    PUSH(msg);
    CALL(print_str);
    POP(n);
    printf("\n");
    EXIT();
ENDP
#endif

/*
    가산기 출력 (+, -, /, *)
*/
#if DTCASE == 2
PROC(main)
    MOVL(a, 10);
    MOVL(b, 5);

    ADD(a, b); // a = 10 + 5
    PUSH(a);
    CALL(print_int);
    POP(n);
    printf("\n");

    MOVL(a, 10);
    MOVL(b, 5);

    SUB(a, b); // a = 10 - 5
    PUSH(a);
    CALL(print_int);
    POP(n);
    printf("\n");

    MOVL(a, 10);
    MOVL(b, 5);

    MUL(a, b); // a = 10 * 5
    PUSH(a);
    CALL(print_int);
    POP(n);
    printf("\n");

    MOVL(a, 10);
    MOVL(b, 5);

    DIV(a, b); // a = 10 / 5
    PUSH(a);
    CALL(print_int);
    POP(n);
    printf("\n");

    EXIT();
ENDP
#endif

/*
    랜덤으로 선택하여 do-while문으로 가산기 출력, exit를 입력하면 종료
*/
#if DTCASE == 3
PROC(main)
    srand(time(0));
    STRING input_msg = "This Program is Random Choice Calculator.\n";
    PUSH(input_msg);
    CALL(print_str);
    POP(n);
    STRING msg = "Enter 'exit' to quit.\n";
    PUSH(msg);
    CALL(print_str);
    POP(n);

    char input[10];
    INT num1, num2;
    do {
        printf("Enter two numbers (or 'exit' to quit):\n");
        if (scanf("%9s", input) != 1 || strcmp(input, "exit") == 0) break;
        num1 = atoi(input);

        if (scanf("%9s", input) != 1 || strcmp(input, "exit") == 0) break;
        num2 = atoi(input);

        MOVL(a, num1);
        MOVL(b, num2);

        int choice = rand() % 4;

        switch (choice) {
            case 0:
                ADD(a, b);
                printf("Operation: ADD\n");
                break;
            case 1:
                SUB(a, b);
                printf("Operation: SUB\n");
                break;
            case 2:
                MUL(a, b);
                printf("Operation: MUL\n");
                break;
            case 3:
                DIV(a, b);
                printf("Operation: DIV\n");
                break;
        }

        PUSH(a);
        CALL(print_int);
        POP(n);
        printf("\n");

        // 스택 포인터를 초기 상태로 되돌림
        sp = MAX_MEMORY_LEN;
    } while (1);

    EXIT();
ENDP
#endif

/*
    비재귀적 피보나치 수열 계산
*/
#if DTCASE == 4
PROC(main)
    MOVL(a, 10);
    if (a == 0) {
        MOVL(b, 0);
    } else if (a == 1) {
        MOVL(b, 1);
    } else {
        INT fib1 = 0, fib2 = 1, temp, i;
        LOOP(i, 2, a+1)
            MOVL(temp, fib1);
            ADD(fib1, fib2);
            MOVL(fib2, temp);
        ENDLOOP(i)
        MOVL(b, fib1);
    }
    PUSH(b);
    CALL(print_int);
    POP(n);
    printf("\n");
    EXIT();
ENDP
#endif

/*
    for문으로 1부터 10까지의 덧셈 구하기
*/
#if DTCASE == 5
PROC(main)
    MOVL(a, 0);
    INT i;
    LOOP(i, 1, 11)
        ADD(a, i);
    ENDLOOP(i)

    PUSH(a);
    CALL(print_int);
    POP(n);
    printf("\n");
    EXIT();
ENDP
#endif

/*
    포인터 연산 어셈블리로 하기(주소 값 대입으로 변형)
*/
#if DTCASE == 6
PROC(main)
    INT x = 10;
    INT* p;
    LEA(p, &x);
    SETL(p, 20);

    PUSH(x);
    CALL(print_int);
    POP(n);
    printf("\n");
    EXIT();
ENDP
#endif

/*
    게임 캐릭터 구조체를 만들어서 실제 게임하는 것처럼 연산해보기
*/
#if DTCASE == 7
typedef struct {
    INT hp;
    INT attack;
    INT defense;
} Character;

PROC(main)
    Character hero = { 100, 20, 10 };
    Character monster = { 80, 15, 5 };

    // Hero attacks monster
    SUB(monster.hp, hero.attack);
    MOVL(a, monster.hp);
    PUSH(a);
    CALL(print_int);
    POP(n);
    printf(" - Monster HP after attack\n");

    // Monster attacks hero
    SUB(hero.hp, monster.attack);
    MOVL(a, hero.hp);
    PUSH(a);
    CALL(print_int);
    POP(n);
    printf(" - Hero HP after attack\n");

    EXIT();
ENDP
#endif

// monster.hp = monster.hp - hero.attack = 80 - 20 = 60
// hero.hp = hero.hp - monster.attack = 100 - 15 = 85