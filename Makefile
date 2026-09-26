CC ?= cc
EMCC ?= emcc
CFLAGS ?= -O2 -std=c11 -Wall -Wextra -Wpedantic
SDL_CFLAGS := $(shell pkg-config --cflags sdl2 SDL2_ttf)
SDL_LIBS := $(shell pkg-config --libs sdl2 SDL2_ttf)

WEB_FONTS := web/fonts/DejaVuSerif.ttf web/fonts/DejaVuSansMono.ttf \
	web/fonts/LiberationSerif-Italic.ttf web/fonts/FONT-LICENSES.txt

.PHONY: all clean run test web
all: space

space: main.c bodies.h
	$(CC) $(CFLAGS) $(SDL_CFLAGS) -o $@ main.c $(SDL_LIBS) -lm

run: space
	./space

test: space
	SDL_VIDEODRIVER=dummy ./space --smoke

web: web/index.html

web/index.html: Makefile main.c bodies.h web/shell.html web/site.css $(WEB_FONTS)
	$(EMCC) main.c -O2 -std=c11 -sUSE_SDL=2 -sUSE_SDL_TTF=2 \
		-sALLOW_MEMORY_GROWTH=1 -sFORCE_FILESYSTEM -sNO_EXIT_RUNTIME=1 \
		-sMINIFY_HTML=0 \
		--preload-file web/fonts@/fonts --shell-file web/shell.html \
		-o web/index.html

clean:
	rm -f space web/index.html web/index.js web/index.wasm web/index.data
