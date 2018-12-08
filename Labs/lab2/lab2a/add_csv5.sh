#!/bin/bash

touch lab2_add.csv
#truncate -s 0 lab2_add.csv

./lab2_add --threads=1 --iterations=10000 >> lab2_add.csv
./lab2_add --threads=2 --iterations=10000 >> lab2_add.csv
./lab2_add --threads=4 --iterations=10000 >> lab2_add.csv
./lab2_add --threads=8 --iterations=10000 >> lab2_add.csv
./lab2_add --threads=12 --iterations=10000 >> lab2_add.csv

./lab2_add --threads=1 --iterations=10000 --sync=m >> lab2_add.csv
./lab2_add --threads=2 --iterations=10000 --sync=m >> lab2_add.csv
./lab2_add --threads=4 --iterations=10000 --sync=m >> lab2_add.csv
./lab2_add --threads=8 --iterations=10000 --sync=m >> lab2_add.csv
./lab2_add --threads=12 --iterations=10000 --sync=m >> lab2_add.csv

./lab2_add --threads=1 --iterations=10000 --sync=c >> lab2_add.csv
./lab2_add --threads=2 --iterations=10000 --sync=c >> lab2_add.csv
./lab2_add --threads=4 --iterations=10000 --sync=c >> lab2_add.csv
./lab2_add --threads=8 --iterations=10000 --sync=c >> lab2_add.csv
./lab2_add --threads=12 --iterations=10000 --sync=c >> lab2_add.csv

./lab2_add --threads=1 --iterations=10000 --sync=s >> lab2_add.csv
./lab2_add --threads=2 --iterations=10000 --sync=s >> lab2_add.csv
./lab2_add --threads=4 --iterations=10000 --sync=s >> lab2_add.csv
./lab2_add --threads=8 --iterations=10000 --sync=s >> lab2_add.csv
./lab2_add --threads=12 --iterations=10000 --sync=s >> lab2_add.csv

# ./lab2_add.gp
