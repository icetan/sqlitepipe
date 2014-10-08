CC=gcc
CFLAGS=-fPIC -Isqlite3

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	CFLAGS += -shared
endif
ifeq ($(UNAME_S),Darwin)
	CFLAGS += -bundle
endif

pipe.sqlext: sqlitepipe.c
	$(CC) $(CFLAGS) -o $@ $<

all: pipe.sqlext

.PHONY: clean
clean:
	rm -rf pipe.sqlext
