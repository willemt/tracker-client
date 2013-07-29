CONTRIB_DIR = ..
BENCODE_DIR = $(CONTRIB_DIR)/CHeaplessBencodeReader

GCOV_OUTPUT = *.gcda *.gcno *.gcov 
GCOV_CCFLAGS = -fprofile-arcs -ftest-coverage
SHELL  = /bin/bash
CC     = gcc
CCFLAGS = -g -O2 -Wall -Werror -W -fno-omit-frame-pointer -fno-common -fsigned-char $(GCOV_CCFLAGS) -I$(BENCODE_DIR) -Ihttp-parser

all: tests

cheap:
	mkdir -p $(BENCODE_DIR)/.git
	git --git-dir=$(BENCODE_DIR)/.git init 
	pushd $(BENCODE_DIR); git pull git@github.com:willemt/CHeaplessBencodeReader.git; popd

download-contrib: cheap

main.c:
	if test -d $(BENCODE_DIR); \
	then echo have contribs; \
	else make download-contrib; \
	fi
	sh make-tests.sh > main.c

tests: main.c tracker_client.o tracker_http.c http-parser\http_parser.c bt_tracker_client_response_reader.c test_url.c url_encoder.c CuTest.c main.c $(BENCODE_DIR)/bencode.c 
	$(CC) $(CCFLAGS) -o $@ $^
	./tests
	#gcov main.c test_bt_tracker_client.c bt_tracker_client.c

bt_tracker_client.o: bt_tracker_client.c 
	$(CC) $(CCFLAGS) -c -o $@ $^

clean:
	rm -f main.c *.o tests $(GCOV_OUTPUT)
