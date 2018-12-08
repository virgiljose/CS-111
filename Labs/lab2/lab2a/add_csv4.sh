#!/bin/bash

touch lab2_add.csv
#truncate -s 0 lab2_add.csv
./lab2_add --threads=2 --iterations=10000 --sync=m >> lab2_add.csv
./lab2_add --threads=4 --iterations=10000 --sync=m >> lab2_add.csv
./lab2_add --threads=8 --iterations=10000 --sync=m >> lab2_add.csv
./lab2_add --threads=12 --iterations=10000 --sync=m >> lab2_add.csv
./lab2_add --threads=2 --iterations=1000 --sync=s >> lab2_add.csv
./lab2_add --threads=4 --iterations=1000 --sync=s >> lab2_add.csv
./lab2_add --threads=8 --iterations=1000 --sync=s >> lab2_add.csv
./lab2_add --threads=12 --iterations=1000 --sync=s >> lab2_add.csv
./lab2_add --threads=2 --iterations=10000 --sync=c >> lab2_add.csv
./lab2_add --threads=4 --iterations=10000 --sync=c >> lab2_add.csv
./lab2_add --threads=8 --iterations=10000 --sync=c >> lab2_add.csv
./lab2_add --threads=12 --iterations=10000 --sync=c >> lab2_add.csv

./lab2_add --threads=2 --iterations=10000 --yield --sync=m >> lab2_add.csv
./lab2_add --threads=4 --iterations=10000 --yield --sync=m >> lab2_add.csv
./lab2_add --threads=8 --iterations=10000 --yield --sync=m >> lab2_add.csv
./lab2_add --threads=12 --iterations=10000 --yield --sync=m >> lab2_add.csv
./lab2_add --threads=2 --iterations=1000 --yield --sync=s >> lab2_add.csv
./lab2_add --threads=4 --iterations=1000 --yield --sync=s >> lab2_add.csv
./lab2_add --threads=8 --iterations=1000 --yield --sync=s >> lab2_add.csv
./lab2_add --threads=12 --iterations=1000 --yield --sync=s >> lab2_add.csv
./lab2_add --threads=2 --iterations=10000 --yield --sync=c >> lab2_add.csv
./lab2_add --threads=4 --iterations=10000 --yield --sync=c >> lab2_add.csv
./lab2_add --threads=8 --iterations=10000 --yield --sync=c >> lab2_add.csv
./lab2_add --threads=12 --iterations=10000 --yield --sync=c >> lab2_add.csv

# ./lab2_add.gp
