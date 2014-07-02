GCOV_OUTPUT = *.gcda *.gcno *.gcov 
GCOV_CCFLAGS = -fprofile-arcs -ftest-coverage
CC     = gcc
SHELL  = /bin/bash
INCLUDES = $(shell ls deps | sed 's/^/-Ideps\//')
DEPS_SRC = $(shell find deps -name *.c)
CCFLAGS = -g -O2 -Wall -Werror -fno-omit-frame-pointer -fno-common -fsigned-char $(GCOV_CCFLAGS) -I$(INCLUDES) -Iinclude


ifeq ($(OS),Windows_NT)
    CCFLAGS += -D WIN32
    ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
        CCFLAGS += -D AMD64
    endif
    ifeq ($(PROCESSOR_ARCHITECTURE),x86)
        CCFLAGS += -D IA32
    endif
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        CCFLAGS += -D LINUX
    endif
    ifeq ($(UNAME_S),Darwin)
        CCFLAGS += -D OSX
    endif
    UNAME_P := $(shell uname -p)
    ifeq ($(UNAME_P),x86_64)
        CCFLAGS += -D AMD64
    endif
    ifneq ($(filter %86,$(UNAME_P)),)
        CCFLAGS += -D IA32
    endif
    ifneq ($(filter arm%,$(UNAME_P)),)
        CCFLAGS += -D ARM
    endif
endif



all: test

main.c:
	sh tests/make-tests.sh tests/test_*.c > main.c

test: $(DEPS_SRC) src/tracker_client.c src/tracker_http.c src/tracker_http_response_reader.c tests/CuTest.c main.c
	$(CC) $(CCFLAGS) -o $@ $^
	./tests
	#gcov main.c test_bt_tracker_client.c 

clean:
	rm -f main.c *.o test $(GCOV_OUTPUT)
