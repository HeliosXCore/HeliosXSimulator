CC	:= g++
NAME 		:= HeliosXSimulator
EXAMPLE 	:= emulator
SRCS		:= $(wildcard src/*.cpp)
OBJS 		:= $(patsubst src/%.cpp,build/%.o,$(SRCS))
IFLAGS		:= -Iinclude -I3rd-party/fmt/include \
			   -I/opt/homebrew/Cellar/verilator/5.016/share/verilator/include \
			   -I/home/vscode/verilator-5.020/include
LDFLAGS		:= -L3rd-party/HeliosXEmulator/build -lHeliosXEmulator \
			   -L3rd-party/fmt/build -lfmt \
			   -lstdc++
DFLAGS		:= -DFMT_HEADER_ONLY

DEBUG		?= N

BINARY ?= build/$(NAME)

ifeq ($(DEBUG),Y)
	DFLAGS += -DDEBUG
endif

.PHONY: build clean

all: $(OBJS)

build/%.o: src/%.cpp
	@mkdir -p build
	@$(CC) $(IFLAGS) $(DFLAGS) -std=c++17 -c $< -o $@

build: $(OBJS)

$(BINARY).a: $(OBJS)
	@echo + AR $@
	@ar rcs $@ $^
	@mv $(BINARY).a build/lib$(NAME).a

static : $(BINARY).a

example: build
	@$(CC) examples/$(EXAMPLE).cpp $(OBJS) $(IFLAGS) $(DFLAGS) $(LDFLAGS) -o build/$(EXAMPLE) -std=c++17
	@./build/$(EXAMPLE)

clean:
	rm -rf build
	