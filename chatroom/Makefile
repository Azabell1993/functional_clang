# Makefile

CC = gcc
EMCC = emcc
CFLAGS = -O2
EMCC_FLAGS = -O2 -s EXPORTED_FUNCTIONS='["_send_message"]' -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]' -s EXTRA_EXPORTED_RUNTIME_METHODS='["FS"]'
SRC_DIR = src
BUILD_DIR = build

# 대상 파일들
SERVER_SRC = $(SRC_DIR)/chat_server.c
CLIENT_SRC = $(SRC_DIR)/chat_client.c
WASM_SRC = $(SRC_DIR)/chat_room.c

SERVER_BIN = $(BUILD_DIR)/chat_server
CLIENT_BIN = $(BUILD_DIR)/chat_client
WASM_BIN = $(BUILD_DIR)/chat_room.js

# 기본 빌드 타겟
all: $(SERVER_BIN) $(CLIENT_BIN) $(WASM_BIN)

# 빌드 서버
$(SERVER_BIN): $(SERVER_SRC)
	$(CC) $(CFLAGS) -o $@ $^

# 빌드 클라이언트
$(CLIENT_BIN): $(CLIENT_SRC)
	$(CC) $(CFLAGS) -o $@ $^

# WebAssembly 모듈 빌드
$(WASM_BIN): $(WASM_SRC)
	$(EMCC) $(EMCC_FLAGS) $^ -o $@ -s MODULARIZE=1 -s 'EXPORT_NAME="createModule"' --preload-file $(SRC_DIR)@/ --js-library $(SRC_DIR)/chat_room.js

# 클린 타겟
clean:
	rm -rf $(BUILD_DIR)/*

# 빌드 디렉토리 생성
$(shell mkdir -p $(BUILD_DIR))

.PHONY: all clean
