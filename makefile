CFLAGS=-Wextra -Iinclude
TARGET=bin/test
RM=rm -rf
OBJ=$(patsubst src/%,bin/%.o,$(basename $(wildcard src/*)))
DBGR=lldb

all: $(TARGET)

test: $(TARGET)
	$(TARGET)

debug: CFLAGS+=-g
debug: TARGET=bin/debug
debug: clean $(TARGET)
	$(DBGR) $(TARGET)

release: CFLAGS+=-Ofast
release: TARGET=bin/release
release: clean $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(LIBS) $^ -o $(TARGET)

bin/%.o: CFLAGS+=-c
bin/%.o: src/%.c bin
	$(CC) $< $(CFLAGS) -o $@

bin:
	mkdir bin

clean:
	$(RM) bin

.PHONY: all test debug release clean
