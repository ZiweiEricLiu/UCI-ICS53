CC := gcc

LIB := $(shell find lib -type f -name *.o)
INC := -I include

DFLAGS := -g -DDEBUG -DCOLOR
CFLAGS := $(INC) -Wno-pointer-to-int-cast -Wno-int-to-pointer-cast $(DFLAGS)

.PHONY: clean all setup

all: 53csv lltest checksum packetlist

53csv: setup
	$(CC) $(CFLAGS) $(LIB) src/helpers.c src/hw3_p1.c src/53csv_main.c -o bin/53csv

lltest: setup
	$(CC) $(CFLAGS) $(LIB) src/helpers.c src/hw3_p1.c tests/lltest_main.c -o bin/lltest

checksum: setup
	$(CC) $(CFLAGS) $(LIB) src/helpers.c src/hw3_p2.c tests/checksum_main.c -o bin/checksum

packetlist: setup
	$(CC) $(CFLAGS) $(LIB) src/helpers.c src/hw3_p2.c src/hw3_p3.c tests/packetlist_main.c -o bin/packetlist

setup:
	mkdir -p bin

clean:
	$(RM) -r bin
