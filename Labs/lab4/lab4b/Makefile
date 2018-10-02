# NAME: Virgil Jose
# EMAIL: xxxxxxxxx
# ID: xxxxxxxxx

CC=gcc
CFLAGS=-lmraa -Wall -Wextra

default:
	$(CC) $(CFLAGS) -o lab4b lab4b.c -lm

check:
	printf "SCALE=C\nSCALE=F\nSTART\nSTOP\nPERIOD=2\nOFF\n" | ./lab4b --log=check.txt
	if ! grep -Eoq '[0-9]+\.[0-9]' check.txt ; then \
		echo "Smoke test failed. Program did not report a temperature.\n" ; \
		exit 1 ; \
	fi \

	if ! grep -q 'SCALE=C' check.txt ; then \
		echo "Smoke test failed. Program did not change scale to Celsius.\n" ; \
		exit 1 ; \
	fi \

	if ! grep -q 'SCALE=F' check.txt ; then \
		echo "Smoke test failed. Program did not change scale to Fahrenheit.\n" ; \
		exit 1 ; \
	fi \

	if ! grep -q 'PERIOD=2' check.txt ; then \
		echo "Smoke test failed. Program did not change sampling period.\n" ; \
		exit 1 ; \
	fi \

	if ! grep -q 'STOP' check.txt ; then \
		echo "Smoke test failed. Program did not stop output.\n" ; \
		exit 1 ; \
	fi \

	if ! grep -q 'START' check.txt ; then \
		echo "Smoke test failed. Program did not restart output.\n" ; \
		exit 1 ; \
	fi \

	if ! grep -q 'OFF' check.txt ; then \
		echo "Smoke test failed. Program did not shutdown.\n" ; \
		exit 1 ; \
	fi \

	rm -f check.txt
	echo "Smoke test passed\n"


clean:
	rm lab4b-xxxxxxxxx.tar.gz

dist:
	tar -czvf lab4b-xxxxxxxxx.tar.gz lab4b.c README Makefile
