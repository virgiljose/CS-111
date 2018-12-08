#!/bin/bash

touch lab2_add.csv
#truncate -s 0 lab2_add.csv
./lab2_add --threads=1 --iterations=1 >> lab2_add.csv
./lab2_add --threads=1 --iterations=10 >> lab2_add.csv
./lab2_add --threads=1 --iterations=100 >> lab2_add.csv
./lab2_add --threads=1 --iterations=1000 >> lab2_add.csv
./lab2_add --threads=1 --iterations=10000 >> lab2_add.csv
./lab2_add --threads=1 --iterations=100000 >> lab2_add.csv

#./lab2_add.gp
