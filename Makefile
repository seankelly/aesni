OPENSSL_PC := $(shell pkg-config --cflags --libs openssl)
CFLAGS = -DDEBUG -O2 -g -Wall -Wextra -Werror
LDFLAGS = $(OPENSSL_PC)

aes: aes.o main.o

main.o: aes.h
