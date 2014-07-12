OPENSSL_PC := $(shell pkg-config --cflags --libs openssl)
CFLAGS = -DDEBUG -Og -g -Wall -Wextra -Werror
LDFLAGS = $(OPENSSL_PC)

aes: aes.o main.o

clean:
	rm -f aes.o main.o aes

main.o: aes.h
