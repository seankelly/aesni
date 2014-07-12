OPENSSL_LIBS = -lcrypto
CFLAGS = -DDEBUG -Og -g -Wall -Wextra -Werror
LDFLAGS = $(OPENSSL_LIBS)

aes: aes.o main.o

clean:
	rm -f aes.o main.o aes

main.o: aes.h
