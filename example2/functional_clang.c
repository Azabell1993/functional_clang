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

    # git clone https://github.com/emscripten-core/emsdk.git
    # cd emsdk
    # ./emsdk install latest
    # ./emsdk activate latest
    # source ./emsdk_env.sh

    # echo 'source /path/to/emsdk/emsdk_env.sh' >> ~/.bashrc
    # source ~/.bashrc

    # make
    # master@DESKTOP-RMTOFBA:/mnt/d/c_functional_2024/functional programming$ make
        emcc -O2 functional_clang.c -o functional_clang.js -s EXPORTED_FUNCTIONS="['_main']" -s EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]'
        cache:INFO: generating system asset: symbol_lists/030c4a0b7369ead78f8bb1cc63b1930b44558dc9.json... (this will be cached in "/mnt/d/c_functional_2024/emsdk/upstream/emscripten/cache/symbol_lists/030c4a0b7369ead78f8bb1cc63b1930b44558dc9.json" for subsequent builds)
        cache:INFO:  - ok


    # python -m http.server
*/

#pragma ONCE
#ifndef DRAFT_H
#define DRAFT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <emscripten/emscripten.h>

// 컴포넌트 매크로 정의
#define component(name) void name(void)

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

// Calculator 생성자 함수 정의
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

// App 생성자 함수 정의
App new_App() {
    App app;
    app.routes = NULL;
    app.route_count = 0;
    app.add_route = add_route;
    app.navigate = navigate;
    return app;
}

// 함수 정의
double add_func(Calculator *self) { return self->a + self->b; }
double subtract_func(Calculator *self) { return self->a - self->b; }
double multiply_func(Calculator *self) { return self->a * self->b; }
double divide_func(Calculator *self) { return self->a / self->b; }

// HomePage 컴포넌트 정의
component(HomePage) {
    printf("Welcome to the Home Page!\n");
}

// CalculatorPage 컴포넌트 정의
component(CalculatorPage) {
    Calculator calc = new_Calculator(10.0, 5.0);

    // 명시적으로 정의된 함수 포인터 사용
    double sum = calc.apply(&calc, add_func);
    printf("Sum: %lf\n", sum); // 15.0

    double difference = calc.apply(&calc, subtract_func);
    printf("Difference: %lf\n", difference); // 5.0

    double product = calc.apply(&calc, multiply_func);
    printf("Product: %lf\n", product); // 50.0

    double quotient = calc.apply(&calc, divide_func);
    printf("Quotient: %lf\n", quotient); // 2.0
}

// Controller 함수 정의
void controller(App *app) {
    app->navigate(app, "/");
    app->navigate(app, "/calculator");
    app->navigate(app, "/non-existent");
}

// EMSCRIPTEN_KEEPALIVE를 사용하여 함수 노출
EMSCRIPTEN_KEEPALIVE
void navigate_path(const char* path) {
    static App app;
    static int initialized = 0;

    if (!initialized) {
        app = new_App();
        app.add_route(&app, "/", HomePage);
        app.add_route(&app, "/calculator", CalculatorPage);
        initialized = 1;
    }
    app.navigate(&app, path);
}

// 메인 함수 보호 플래그
int main_called = 0;

// 메인 함수 정의
int main(int argc, char** argv) {
    if (main_called) {
        return 0;
    }
    main_called = 1;

    App app = new_App();

    app.add_route(&app, "/", HomePage);
    app.add_route(&app, "/calculator", CalculatorPage);

    controller(&app);

    free(app.routes);
    return 0;
}
#endif
