#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <emscripten/emscripten.h>

#define BUF_SIZE 256

// WebSocket이나 다른 방법으로 메시지를 전송하는 JavaScript 함수 호출
void send_message_to_js(const char* message) {
    char js_code[BUF_SIZE];
    snprintf(js_code, sizeof(js_code), "sendMessageToServer('%s');", message);
    emscripten_run_script(js_code);
}

// Emscripten에서 노출할 함수
EMSCRIPTEN_KEEPALIVE
void send_message(const char* message) {
    printf("Sending message: %s\n", message);
    send_message_to_js(message);
}

int main() {
    // Emscripten에서 메인 함수가 빈 상태로 유지되더라도 실행을 보장하기 위한 코드
    emscripten_exit_with_live_runtime();
    return 0;
}