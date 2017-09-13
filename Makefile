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

SOURCE=sqlitepipe.c
OBJECT=$(SOURCE:.c=.$(SUFFIX))

SQLITERC_HEADER=-- Auto load sqlitepipe

$(OBJECT): $(SOURCE)
	$(CC) $(CFLAGS) -o $@ $<

all: $(OBJECT)

.PHONY: uninstall install clean
uninstall:
	sed -i'' '/^$(SQLITERC_HEADER)/{N;d;}' ~/.sqliterc &> /dev/null || true

install: uninstall
	echo $(SQLITERC_HEADER) >> ~/.sqliterc
	echo .load `pwd`/$(OBJECT) >> ~/.sqliterc

clean:
	rm -rf $(OBJECT)
