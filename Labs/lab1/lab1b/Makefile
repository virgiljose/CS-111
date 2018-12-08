# NAME: Virgil Jose
# EMAIL: xxxxxxxxx
# ID: xxxxxxxxx

default: lab1b-client.c lab1b-server.c
	@gcc -lz -Wall -Wextra -o lab1b-client lab1b-client.c 
	@gcc -lz -Wall -Wextra -o lab1b-server lab1b-server.c

client: lab1b-client.c
	@gcc -lz -Wall -Wextra -o lab1b-client lab1b-client.c

server: lab1b-server.c 
	@gcc -lz -Wall -Wextra -o lab1b-server lab1b-server.c

clean:
	@rm -rf lab1b-client lab1b-server *.tar.gz

dist: default
	@tar -czf lab1b-904765891.tar.gz lab1b-client.c lab1b-server.c Makefile README
