# Makefile
EMCC = emcc
CFLAGS = -O2

all: functional_clang.js

functional_clang.js: functional_clang.c
	$(EMCC) $(CFLAGS) functional_clang.c -o functional_clang.js -s EXPORTED_FUNCTIONS="['_main']" -s EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]'

clean:
	rm -f functional_clang.js functional_clang.wasm functional_clang.html
