# NAME: Virgil Jose
# EMAIL: xxxxxxxxx
# ID: xxxxxxxxx

CC = gcc
CFLAGS = -Wall -Wextra

default:
	$(CC) $(CFLAGS) -lmraa -o lab4c_tcp lab4c_tcp.c -lm
	$(CC) $(CFLAGS) -lmraa -o lab4c_tls lab4c_tls.c -lm -lssl -lcrypto

dist:
	tar -czf lab4c-xxxxxxxxx.tar.gz lab4c_tcp.c lab4c_tls.c Makefile README

clean:
	rm -rf lab4c_tcp lab4c_tls lab4c-xxxxxxxxx.tar.gz
