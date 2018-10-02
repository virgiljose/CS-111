# NAME: Virgil Jose
# EMAIL: xxxxxxxxx
# ID: xxxxxxxxx

build:
	@gcc -Wall -Wextra -lprofiler -lpthread -g -o lab2_list lab2_list.c SortedList.c

tests: build
	@touch lab2b_list.csv
	./lab2_list --iterations=1000 --sync=m --threads=1 >> lab2b_list.csv
	./lab2_list --iterations=1000 --sync=m --threads=2 >> lab2b_list.csv
	./lab2_list --iterations=1000 --sync=m --threads=4 >> lab2b_list.csv
	./lab2_list --iterations=1000 --sync=m --threads=8 >> lab2b_list.csv
	./lab2_list --iterations=1000 --sync=m --threads=12 >> lab2b_list.csv
	./lab2_list --iterations=1000 --sync=m --threads=16 >> lab2b_list.csv
	./lab2_list --iterations=1000 --sync=m --threads=24 >> lab2b_list.csv

	./lab2_list --iterations=1000 --sync=s --threads=1 >> lab2b_list.csv
	./lab2_list --iterations=1000 --sync=s --threads=2 >> lab2b_list.csv
	./lab2_list --iterations=1000 --sync=s --threads=4 >> lab2b_list.csv
	./lab2_list --iterations=1000 --sync=s --threads=8 >> lab2b_list.csv
	./lab2_list --iterations=1000 --sync=s --threads=12 >> lab2b_list.csv
	./lab2_list --iterations=1000 --sync=s --threads=16 >> lab2b_list.csv
	./lab2_list --iterations=1000 --sync=s --threads=24 >> lab2b_list.csv

	./lab2_list --yield=id --lists=4 --threads=1 --iterations=1 >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=1 --iterations=2 >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=1 --iterations=4 >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=1 --iterations=8 >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=1 --iterations=16 >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=4 --iterations=1 >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=4 --iterations=2 >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=4 --iterations=4 >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=4 --iterations=8 >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=4 --iterations=16 >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=8 --iterations=1 >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=8 --iterations=2 >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=8 --iterations=4 >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=8 --iterations=8 >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=8 --iterations=16 >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=12 --iterations=1 >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=12 --iterations=2 >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=12 --iterations=4 >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=12 --iterations=8 >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=12 --iterations=16 >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=16 --iterations=1 >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=16 --iterations=2 >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=16 --iterations=4 >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=16 --iterations=8 >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=16 --iterations=16 >> lab2b_list.csv

	./lab2_list --yield=id --lists=4 --sync=s --threads=1 --iterations=10 >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --sync=s --threads=1 --iterations=20 >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --sync=s --threads=1 --iterations=40 >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --sync=s --threads=1 --iterations=80 >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --sync=s --threads=4 --iterations=10 >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --sync=s --threads=4 --iterations=20 >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --sync=s --threads=4 --iterations=40 >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --sync=s --threads=4 --iterations=80 >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --sync=s --threads=8 --iterations=10 >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --sync=s --threads=8 --iterations=20 >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --sync=s --threads=8 --iterations=40 >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --sync=s --threads=8 --iterations=80 >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --sync=s --threads=12 --iterations=10 >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --sync=s --threads=12 --iterations=20 >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --sync=s --threads=12 --iterations=40 >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --sync=s --threads=12 --iterations=80 >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --sync=s --threads=16 --iterations=10 >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --sync=s --threads=16 --iterations=20 >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --sync=s --threads=16 --iterations=40 >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --sync=s --threads=16 --iterations=80 >> lab2b_list.csv

	./lab2_list --yield=id --lists=4 --sync=m --threads=1 --iterations=10 >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --sync=m --threads=1 --iterations=20 >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --sync=m --threads=1 --iterations=40 >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --sync=m --threads=1 --iterations=80 >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --sync=m --threads=4 --iterations=10 >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --sync=m --threads=4 --iterations=20 >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --sync=m --threads=4 --iterations=40 >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --sync=m --threads=4 --iterations=80 >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --sync=m --threads=8 --iterations=10 >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --sync=m --threads=8 --iterations=20 >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --sync=m --threads=8 --iterations=40 >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --sync=m --threads=8 --iterations=80 >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --sync=m --threads=12 --iterations=10 >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --sync=m --threads=12 --iterations=20 >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --sync=m --threads=12 --iterations=40 >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --sync=m --threads=12 --iterations=80 >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --sync=m --threads=16 --iterations=10 >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --sync=m --threads=16 --iterations=20 >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --sync=m --threads=16 --iterations=40 >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --sync=m --threads=16 --iterations=80 >> lab2b_list.csv

	./lab2_list --iterations=1000 --sync=m --threads=1 --lists=4 >> lab2b_list.csv
	./lab2_list --iterations=1000 --sync=m --threads=1 --lists=8 >> lab2b_list.csv
	./lab2_list --iterations=1000 --sync=m --threads=1 --lists=16 >> lab2b_list.csv
	./lab2_list --iterations=1000 --sync=m --threads=2 --lists=4 >> lab2b_list.csv
	./lab2_list --iterations=1000 --sync=m --threads=2 --lists=8 >> lab2b_list.csv
	./lab2_list --iterations=1000 --sync=m --threads=2 --lists=16 >> lab2b_list.csv
	./lab2_list --iterations=1000 --sync=m --threads=4 --lists=4 >> lab2b_list.csv
	./lab2_list --iterations=1000 --sync=m --threads=4 --lists=8 >> lab2b_list.csv
	./lab2_list --iterations=1000 --sync=m --threads=4 --lists=16 >> lab2b_list.csv
	./lab2_list --iterations=1000 --sync=m --threads=8 --lists=4 >> lab2b_list.csv
	./lab2_list --iterations=1000 --sync=m --threads=8 --lists=8 >> lab2b_list.csv
	./lab2_list --iterations=1000 --sync=m --threads=8 --lists=16 >> lab2b_list.csv
	./lab2_list --iterations=1000 --sync=m --threads=12 --lists=4 >> lab2b_list.csv
	./lab2_list --iterations=1000 --sync=m --threads=12 --lists=8 >> lab2b_list.csv
	./lab2_list --iterations=1000 --sync=m --threads=12 --lists=16 >> lab2b_list.csv
	./lab2_list --iterations=1000 --sync=s --threads=1 --lists=4 >> lab2b_list.csv
	./lab2_list --iterations=1000 --sync=s --threads=1 --lists=8 >> lab2b_list.csv
	./lab2_list --iterations=1000 --sync=s --threads=1 --lists=16 >> lab2b_list.csv
	./lab2_list --iterations=1000 --sync=s --threads=2 --lists=4 >> lab2b_list.csv
	./lab2_list --iterations=1000 --sync=s --threads=2 --lists=8 >> lab2b_list.csv
	./lab2_list --iterations=1000 --sync=s --threads=2 --lists=16 >> lab2b_list.csv
	./lab2_list --iterations=1000 --sync=s --threads=4 --lists=4 >> lab2b_list.csv
	./lab2_list --iterations=1000 --sync=s --threads=4 --lists=8 >> lab2b_list.csv
	./lab2_list --iterations=1000 --sync=s --threads=4 --lists=16 >> lab2b_list.csv
	./lab2_list --iterations=1000 --sync=s --threads=8 --lists=4 >> lab2b_list.csv
	./lab2_list --iterations=1000 --sync=s --threads=8 --lists=8 >> lab2b_list.csv
	./lab2_list --iterations=1000 --sync=s --threads=8 --lists=16 >> lab2b_list.csv
	./lab2_list --iterations=1000 --sync=s --threads=12 --lists=4 >> lab2b_list.csv
	./lab2_list --iterations=1000 --sync=s --threads=12 --lists=8 >> lab2b_list.csv
	./lab2_list --iterations=1000 --sync=s --threads=12 --lists=16 >> lab2b_list.csv


profile: build
	@LDPRELOAD=/usr/lib64/libprofiler.so CPUPROFILE=lab2_list.out ./lab2_list --threads=12 --iterations=1000 --sync=s
	@touch profile.out
	@pprof --list=thread_fctn ./lab2_list lab2_list.out > profile.out
	@rm lab2_list.out

graphs: clean tests
	@gnuplot lab2_list.gp

dist: build profile tests graphs
	@tar -czf lab2b-xxxxxxxxx.tar.gz lab2_list.c SortedList.c SortedList.h Makefile lab2_list.gp lab2b_list.csv profile.out *.png README

clean:
	@rm -rf lab2_list lab2b-xxxxxxxxx.tar.gz
