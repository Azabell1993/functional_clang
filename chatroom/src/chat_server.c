#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUF_SIZE 1024

void error_handling(char *message) {
    perror(message);
    exit(1);
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUF_SIZE];
    int str_len;
    socklen_t client_addr_size;

    // 소켓 생성
    server_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (server_sock == -1)
        error_handling("socket() error");

    // 주소 설정
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    // 소켓 바인딩
    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
        error_handling("bind() error");

    // 소켓 대기 상태로 설정
    if (listen(server_sock, 5) == -1)
        error_handling("listen() error");

    printf("Server listening on port %d...\n", PORT);

    client_addr_size = sizeof(client_addr);
    client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_addr_size);
    if (client_sock == -1)
        error_handling("accept() error");

    while ((str_len = read(client_sock, buffer, BUF_SIZE)) != 0) {
        buffer[str_len] = 0;
        printf("Received: %s\n", buffer);
        write(client_sock, buffer, str_len); // Echo back
    }

    close(client_sock);
    close(server_sock);
    return 0;
}
