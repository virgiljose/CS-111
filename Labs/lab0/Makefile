# NAME: Virgil Jose
# EMAIL: xxxxxxxxx
# ID: xxxxxxxxx

default: lab0.c
	@gcc -g -Wall -Wextra -o lab0 lab0.c

clean:
	@rm -rf lab0 *.tar.gz

dist: default
	@tar -czf lab0-xxxxxxxxx.tar.gz lab0.c Makefile backtrace.png breakpoint.png README

check: default
	@COUNT=0
	@touch file1.txt ; \
	touch file2.txt ; \
	touch secret_input.txt; \
	chmod -r secret_input.txt; \
	touch secret_output.txt; \
	chmod -w secret_output.txt \

# TEST CASES THAT SHOULD RUN NORMALLY (i.e. RETURN 0):

# Input only.
	@./lab0 --input=file1.txt ; \
	if [[ $$? -ne 0 ]] ; then \
		let COUNT++ ; \
	fi \

# Input only (piped).
	@./lab0 < file1.txt ; \
	if [[ $$? -ne 0 ]] ; then \
		let COUNT++ ; \
	fi \

# Both input and output. Input specified before output.
	@./lab0 --input=file1.txt --output=file2.txt ; \
	if [[ $$? -ne 0 ]] ; then \
		let COUNT++ ; \
	fi \

# Both input and output. Input specified after output.
	@./lab0 --output=file2.txt --input=file1.txt ; \
	if [[ $$? -ne 0 ]] ; then \
		let COUNT++ ; \
	fi \

# Both input and output. Input piped.
	@./lab0 --output=file2.txt < file1.txt ; \
	if [[ $$? -ne 0 ]] ; then \
		let COUNT++ ; \
	fi \

# Both input and output. Output piped.
	@./lab0 --input=file1.txt > file2.txt ; \
	if [[ $$? -ne 0 ]] ; then \
		let COUNT++ ; \
	fi \

# Both input and output. Input and output piped.
	@./lab0 < file1.txt > file2.txt ; \
	if [[ $$? -ne 0 ]] ; then \
		let COUNT++ ; \
	fi \

# TEST CASES THAT SHOULD EXIT WITH AN ERROR CODE:

# Could not test unrecognized argument case, because doing do causes a make error, i.e:
# 	./lab0 --bogus
# 	./lab0: unrecognized option '--bogus'
# 	Usage: --input=input_file --output=output_filemake: *** [check] Error 1

# Unable to Open Input file (because no such file): Should Return 2
	@./lab0 --input=fake_file.txt --output=file2.txt  2> /dev/null ; \
	if [[ $$? -ne 2 ]] ; then \
		let COUNT++ ; \
	fi \

# Unable to Open Input File (because no read permission): Should Return 2
	@./lab0 --input=secret_input.txt --output=file2.txt  2> /dev/null ; \
	if [[ $$? -ne 2 ]] ; then \
		let COUNT++ ; \
	fi \

# Unable to Open (Create) Output File (because no permission to write): Should Return 3
	@./lab0 --input=file1.txt --output=secret_output.txt  2> /dev/null ; \
	if [[ $$? -ne 3 ]] ; then \
		let COUNT++ ; \
	fi \

# Caught and received SIGSEGV: Should Return 4
	@./lab0 --segfault --catch < file1.txt > file2.txt 2> /dev/null ; \
	if [[ $$? -ne 4 ]] ; then \
		let COUNT++ ; \
	fi \

	@./lab0 --input=file1.txt --output=file2.txt --segfault --catch  2> /dev/null ; \
	if [[ $$? -ne 4 ]] ; then \
		let COUNT++ ; \
	fi \

# Remove files that were created only to test the test cases:
	@chmod +w secret_output.txt ; \
	rm file1.txt file2.txt secret_input.txt secret_output.txt ; \

# Check the value of COUNT. If it is greater than 0, then at least one test failed.
	@ if [[ "$(COUNT)" -eq 0 ]] ; then \
		echo "Passed all tests." ; \
	fi
