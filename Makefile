CC	:= g++

EXAMPLE 	:= emulator
SRCS		:= $(wildcard src/*.cpp)
OBJS 		:= $(patsubst src/%.cpp,build/%.o,$(SRCS))
IFLAGS		:= -Iinclude -I3rd-party/fmt/include \
			   -I/opt/homebrew/Cellar/verilator/5.016/share/verilator/include
LDFLAGS		:= -L3rd-party/HeliosXEmulator/build -lHeliosXEmulator \
			   -L3rd-party/fmt/build -lfmt \
			   -lstdc++
DFLAGS		:= -DFMT_HEADER_ONLY

DEBUG		?= N

ifeq ($(DEBUG),Y)
	DFLAGS += -DDEBUG
endif

.PHONY: build clean

all: $(OBJS)

build/%.o: src/%.cpp
	@mkdir -p build
	@$(CC) $(IFLAGS) -std=c++17 -c $< -o $@

build: $(OBJS)

example: build
	@$(CC) examples/$(EXAMPLE).cpp $(OBJS) $(IFLAGS) $(DFLAGS) $(LDFLAGS) -o build/$(EXAMPLE) -std=c++17
	@./build/$(EXAMPLE)

clean:
	rm -rf build
	