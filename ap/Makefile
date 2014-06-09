PREFIX=./staging_dir
GCC=$(PREFIX)/toolchain/bin/mips-openwrt-linux-gcc
LD=$(PREFIX)/toolchain/bin/mips-openwrt-linux-ld
GCCFLAGS=-Wall -O2 -I. -I$(PREFIX)/toolchain/include/ -I$(PREFIX)/target/usr/include/
LDFLAGS=-L$(PREFIX)/toolchain/lib/ -L$(PREFIX)/target/usr/lib/ -lpcap -lm -lcurl -lpolarssl -lz -lcrypto -pthread -lmicrohttpd

TARGET=ap_daemon.bin

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

all: $(TARGET)


$(TARGET): $(OBJ)
	$(GCC) $(LDFLAGS) -o $@ $^

%.o: %.c %.h
	$(GCC) $(GCCFLAGS) -o $@ -c $<

clean:
	rm -f *.o $(TARGET)
