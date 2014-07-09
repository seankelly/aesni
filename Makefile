OPENSSL_PC := $(shell pkg-config --cflags --libs openssl)
CFLAGS = -O2 -g
LDFLAGS = $(OPENSSL_PC)

aes: aes.o main.o
