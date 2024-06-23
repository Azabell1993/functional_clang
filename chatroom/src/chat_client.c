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
    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUF_SIZE];
    int str_len;

    // 소켓 생성
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        error_handling("socket() error");

    // 서버 주소 설정
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(PORT);

    // 서버에 연결
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
        error_handling("connect() error");

    printf("Connected to server on port %d...\n", PORT);

    while (1) {
        printf("Input message (q to quit): ");
        fgets(buffer, BUF_SIZE, stdin);

        if (!strcmp(buffer, "q\n")) {
            break;
        }

        write(sock, buffer, strlen(buffer));
        str_len = read(sock, buffer, BUF_SIZE - 1);
        buffer[str_len] = 0;
        printf("Message from server: %s", buffer);
    }

    close(sock);
    return 0;
}
