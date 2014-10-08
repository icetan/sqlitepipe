CC=gcc
CFLAGS=-fPIC -Isqlite3

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	CFLAGS += -shared
	SUFFIX=so
endif
ifeq ($(UNAME_S),Darwin)
	CFLAGS += -bundle
	SUFFIX=dylib
endif

OBJECT=pipe.$(SUFFIX)

$(OBJECT): sqlitepipe.c
	$(CC) $(CFLAGS) -o $@ $<

all: $(OBJECT)

.PHONY: clean install
install:
	echo .load `pwd`/$(OBJECT) >> ~/.sqliterc

clean:
	rm -rf $(OBJECT)
