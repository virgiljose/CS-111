#!/bin/bash

touch lab2_add.csv
#truncate -s 0 lab2_add.csv
./lab2_add --threads=2 --iterations=100 >> lab2_add.csv
./lab2_add --threads=2 --iterations=1000 >> lab2_add.csv
./lab2_add --threads=2 --iterations=10000 >> lab2_add.csv
./lab2_add --threads=2 --iterations=100000 >> lab2_add.csv
./lab2_add --threads=4 --iterations=100 >> lab2_add.csv
./lab2_add --threads=4 --iterations=1000 >> lab2_add.csv
./lab2_add --threads=4 --iterations=10000 >> lab2_add.csv
./lab2_add --threads=4 --iterations=100000 >> lab2_add.csv
./lab2_add --threads=8 --iterations=100 >> lab2_add.csv
./lab2_add --threads=8 --iterations=1000 >> lab2_add.csv
./lab2_add --threads=8 --iterations=10000 >> lab2_add.csv
./lab2_add --threads=8 --iterations=100000 >> lab2_add.csv
./lab2_add --threads=12 --iterations=100 >> lab2_add.csv
./lab2_add --threads=12 --iterations=1000 >> lab2_add.csv
./lab2_add --threads=12 --iterations=10000 >> lab2_add.csv
./lab2_add --threads=12 --iterations=100000 >> lab2_add.csv

./lab2_add --threads=2 --iterations=10 --yield >> lab2_add.csv
./lab2_add --threads=2 --iterations=20 --yield >> lab2_add.csv
./lab2_add --threads=2 --iterations=40 --yield >> lab2_add.csv
./lab2_add --threads=2 --iterations=80 --yield >> lab2_add.csv
./lab2_add --threads=2 --iterations=100 --yield >> lab2_add.csv
./lab2_add --threads=2 --iterations=1000 --yield >> lab2_add.csv
./lab2_add --threads=2 --iterations=10000 --yield >> lab2_add.csv
./lab2_add --threads=2 --iterations=100000 --yield >> lab2_add.csv
./lab2_add --threads=4 --iterations=10 --yield >> lab2_add.csv
./lab2_add --threads=4 --iterations=20 --yield >> lab2_add.csv
./lab2_add --threads=4 --iterations=40 --yield >> lab2_add.csv
./lab2_add --threads=4 --iterations=80 --yield >> lab2_add.csv
./lab2_add --threads=4 --iterations=100 --yield >> lab2_add.csv
./lab2_add --threads=4 --iterations=1000 --yield >> lab2_add.csv
./lab2_add --threads=4 --iterations=10000 --yield >> lab2_add.csv
./lab2_add --threads=4 --iterations=100000 --yield >> lab2_add.csv
./lab2_add --threads=8 --iterations=10 --yield >> lab2_add.csv
./lab2_add --threads=8 --iterations=20 --yield >> lab2_add.csv
./lab2_add --threads=8 --iterations=40 --yield >> lab2_add.csv
./lab2_add --threads=8 --iterations=80 --yield >> lab2_add.csv
./lab2_add --threads=8 --iterations=100 --yield >> lab2_add.csv
./lab2_add --threads=8 --iterations=1000 --yield >> lab2_add.csv
./lab2_add --threads=8 --iterations=10000 --yield >> lab2_add.csv
./lab2_add --threads=8 --iterations=100000 --yield >> lab2_add.csv
./lab2_add --threads=12 --iterations=10 --yield >> lab2_add.csv
./lab2_add --threads=12 --iterations=20 --yield >> lab2_add.csv
./lab2_add --threads=12 --iterations=40 --yield >> lab2_add.csv
./lab2_add --threads=12 --iterations=80 --yield >> lab2_add.csv
./lab2_add --threads=12 --iterations=100 --yield >> lab2_add.csv
./lab2_add --threads=12 --iterations=1000 --yield >> lab2_add.csv
./lab2_add --threads=12 --iterations=10000 --yield >> lab2_add.csv
./lab2_add --threads=12 --iterations=100000 --yield >> lab2_add.csv

# ./lab2_add.gp
