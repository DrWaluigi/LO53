PREFIX=./staging_dir
GCC=$(PREFIX)/toolchain/bin/mips-openwrt-linux-gcc
LD=$(PREFIX)/toolchain/bin/mips-openwrt-linux-ld
ALLFLAGS=-Wall -Werror -O2 -I$(PREFIX)/toolchain/include/ -I$(PREFIX)/target/usr/include/ -L$(PREFIX)/toolchain/lib/ -L$(PREFIX)/target/usr/lib/ -lpcap -lm -lcurl -lssl -lz -lcrypto -pthread
GCCFLAGS=-Wall -Werror -O2 -I$(PREFIX)/toolchain/include/ -I$(PREFIX)/target/usr/include/
LDFLAGS=-L$(PREFIX)/toolchain/lib/ -L$(PREFIX)/target/usr/lib/ -lpcap -lm -lcurl -lpolarssl -lz -lcrypto -pthread -lmicrohttpd

TARGET=rssi-display

all: $(TARGET)

rssi-display.o: rssi-display.c rssi_list.h
	$(GCC) $(GCCFLAGS) -c -o rssi-display.o rssi-display.c

%.o : %.c %.h
	$(GCC) $(GCCFLAGS) -o $@ -c $<

rssi-display: rssi-display.o rssi_list.o http-server.o pcap-thread.o 
	$(GCC) $(LDFLAGS) -o rssi-display rssi-display.o rssi_list.o http-server.o pcap-thread.o

clean:
	rm -f *.o
	rm -f rssi-display
	rm -f rssi-report
	rm -f openwrt-helloworld

backupclean:
	rm -f *~
