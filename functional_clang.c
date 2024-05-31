/*
    @ Developer : Azabell1993 Github master  
    @ Created   : 2024-05-31
    @ fileName  : functional_clang.c
    @ Purpose   : Advanced C style lambda and component-based programming
    @ Purpose   : 고급 C 스타일 람다 및 컴포넌트 기반 프로그래밍
    
    -- 소개
    1. **컴포넌트 및 람다 매크로**: `component`와 `lambda` 매크로를 사용하여 컴포넌트 기반 및 람다 스타일 프로그래밍을 구현했습니다.
    2. **구조체와 함수 포인터**: `Calculator`, `Route`, `App` 구조체와 함수 포인터를 사용하여 모듈화된 설계와 동적 라우팅을 구현했습니다.

    이 레포는 C 언어의 한계를 극복하고, 고급 언어의 특징을 C 스타일로 구현하려는 시도를 담고 있습니다.
*/

#pragma ONCE
#ifndef DRAFT_H
#define DRAFT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 람다 함수 매크로
#define lambda(return_type, function_body) ({ return_type __fn__ function_body __fn__; })
// 컴포넌트 매크로
#define component(name, body) void name() body

typedef struct Calculator Calculator;
typedef struct Route Route;
typedef struct App App;

// Calculator 구조체 정의
struct Calculator {
    double a;
    double b;
    double (*apply)(Calculator *self, double (*func)(Calculator*));
};

// Route 구조체 정의
struct Route {
    char *path;
    void (*handler)();
};

// App 구조체 정의
struct App {
    Route *routes;
    int route_count;
    void (*add_route)(App *self, const char *path, void (*handler)());
    void (*navigate)(App *self, const char *path);
};

// Calculator 구조체의 apply 함수 정의
double calculator_apply(Calculator *self, double (*func)(Calculator*)) {
    return func(self);
}

// Calculator 생성자 함수
Calculator new_Calculator(double a, double b) {
    Calculator calc;
    calc.a = a;
    calc.b = b;
    calc.apply = calculator_apply;
    return calc;
}

// App 구조체의 add_route 함수 정의
void add_route(App *self, const char *path, void (*handler)()) {
    self->routes = realloc(self->routes, sizeof(Route) * (self->route_count + 1));
    self->routes[self->route_count].path = strdup(path);
    self->routes[self->route_count].handler = handler;
    self->route_count++;
}

// App 구조체의 navigate 함수 정의
void navigate(App *self, const char *path) {
    for (int i = 0; i < self->route_count; i++) {
        if (strcmp(self->routes[i].path, path) == 0) {
            self->routes[i].handler();
            return;
        }
    }
    printf("Route not found: %s\n", path);
}

// App 생성자 함수
App new_App() {
    App app;
    app.routes = NULL;
    app.route_count = 0;
    app.add_route = add_route;
    app.navigate = navigate;
    return app;
}

// HomePage 컴포넌트 정의
component(HomePage, {
    printf("Welcome to the Home Page!\n");
})

// CalculatorPage 컴포넌트 정의
component(CalculatorPage, {
    Calculator calc = new_Calculator(10.0, 5.0);

    double sum = calc.apply(&calc, lambda(double, (Calculator *self) { return self->a + self->b; }));
    printf("Sum: %lf\n", sum); // 15.0

    double difference = calc.apply(&calc, lambda(double, (Calculator *self) { return self->a - self->b; }));
    printf("Difference: %lf\n", difference); // 5.0

    double product = calc.apply(&calc, lambda(double, (Calculator *self) { return self->a * self->b; }));
    printf("Product: %lf\n", product); // 50.0

    double quotient = calc.apply(&calc, lambda(double, (Calculator *self) { return self->a / self->b; }));
    printf("Quotient: %lf\n", quotient); // 2.0
})

// Controller 함수 정의
void controller(App *app) {
    app->navigate(app, "/");
    app->navigate(app, "/calculator");
    app->navigate(app, "/non-existent");
}

// 메인 함수
int main(int argc, char** argv) {
    App app = new_App();
    
    app.add_route(&app, "/", HomePage);
    app.add_route(&app, "/calculator", CalculatorPage);

    controller(&app);

    free(app.routes);
    return 0;
}
#endif
