OPENSSL_LIBS = -lcrypto
CFLAGS = -DDEBUG -O0 -g -Wall -Wextra -maes
LDFLAGS = $(OPENSSL_LIBS)

OBJECT_FILES = aes.o main.o

aes: $(OBJECT_FILES)
	$(CC) $(LDFLAGS) $(OBJECT_FILES) -o $@

clean:
	rm -f $(OBJECT_FILES) aes

main.o: aes.h
