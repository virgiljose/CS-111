#!/bin/bash

touch lab2_list.csv
truncate -s 0 lab2_list.csv

# STEP 1
./lab2_list --threads=1 --iterations=10 >> lab2_list.csv
./lab2_list --threads=1 --iterations=100 >> lab2_list.csv
./lab2_list --threads=1 --iterations=1000 >> lab2_list.csv
./lab2_list --threads=1 --iterations=10000 >> lab2_list.csv
./lab2_list --threads=1 --iterations=20000 >> lab2_list.csv

# STEP 2
./lab2_list --threads=2 --iterations=1 >> lab2_list.csv
./lab2_list --threads=2 --iterations=10 >> lab2_list.csv
./lab2_list --threads=2 --iterations=100 >> lab2_list.csv
./lab2_list --threads=2 --iterations=1000 >> lab2_list.csv
./lab2_list --threads=4 --iterations=1 >> lab2_list.csv
./lab2_list --threads=4 --iterations=10 >> lab2_list.csv
./lab2_list --threads=4 --iterations=100 >> lab2_list.csv
./lab2_list --threads=4 --iterations=1000 >> lab2_list.csv
./lab2_list --threads=8 --iterations=1 >> lab2_list.csv
./lab2_list --threads=8 --iterations=10 >> lab2_list.csv
./lab2_list --threads=8 --iterations=100 >> lab2_list.csv
./lab2_list --threads=8 --iterations=1000 >> lab2_list.csv
./lab2_list --threads=12 --iterations=1 >> lab2_list.csv
./lab2_list --threads=12 --iterations=10 >> lab2_list.csv
./lab2_list --threads=12 --iterations=100 >> lab2_list.csv
./lab2_list --threads=12 --iterations=1000 >> lab2_list.csv

./lab2_list --threads=2 --iterations=1 --yield=i >> lab2_list.csv
./lab2_list --threads=2 --iterations=2 --yield=i >> lab2_list.csv
./lab2_list --threads=2 --iterations=4 --yield=i >> lab2_list.csv
./lab2_list --threads=2 --iterations=8 --yield=i >> lab2_list.csv
./lab2_list --threads=2 --iterations=16 --yield=i >> lab2_list.csv
./lab2_list --threads=2 --iterations=32 --yield=i >> lab2_list.csv
./lab2_list --threads=4 --iterations=1 --yield=i >> lab2_list.csv
./lab2_list --threads=4 --iterations=2 --yield=i >> lab2_list.csv
./lab2_list --threads=4 --iterations=4 --yield=i >> lab2_list.csv
./lab2_list --threads=4 --iterations=8 --yield=i >> lab2_list.csv
./lab2_list --threads=4 --iterations=16 --yield=i >> lab2_list.csv
./lab2_list --threads=4 --iterations=32 --yield=i >> lab2_list.csv
./lab2_list --threads=8 --iterations=1 --yield=i >> lab2_list.csv
./lab2_list --threads=8 --iterations=2 --yield=i >> lab2_list.csv
./lab2_list --threads=8 --iterations=4 --yield=i >> lab2_list.csv
./lab2_list --threads=8 --iterations=8 --yield=i >> lab2_list.csv
./lab2_list --threads=8 --iterations=16 --yield=i >> lab2_list.csv
./lab2_list --threads=8 --iterations=32 --yield=i >> lab2_list.csv
./lab2_list --threads=12 --iterations=1 --yield=i >> lab2_list.csv
./lab2_list --threads=12 --iterations=2 --yield=i >> lab2_list.csv
./lab2_list --threads=12 --iterations=4 --yield=i >> lab2_list.csv
./lab2_list --threads=12 --iterations=8 --yield=i >> lab2_list.csv
./lab2_list --threads=12 --iterations=16 --yield=i >> lab2_list.csv
./lab2_list --threads=12 --iterations=32 --yield=i >> lab2_list.csv

./lab2_list --threads=2 --iterations=1 --yield=d >> lab2_list.csv
./lab2_list --threads=2 --iterations=2 --yield=d >> lab2_list.csv
./lab2_list --threads=2 --iterations=4 --yield=d >> lab2_list.csv
./lab2_list --threads=2 --iterations=8 --yield=d >> lab2_list.csv
./lab2_list --threads=2 --iterations=16 --yield=d >> lab2_list.csv
./lab2_list --threads=2 --iterations=32 --yield=d >> lab2_list.csv
./lab2_list --threads=4 --iterations=1 --yield=d >> lab2_list.csv
./lab2_list --threads=4 --iterations=2 --yield=d >> lab2_list.csv
./lab2_list --threads=4 --iterations=4 --yield=d >> lab2_list.csv
./lab2_list --threads=4 --iterations=8 --yield=d >> lab2_list.csv
./lab2_list --threads=4 --iterations=16 --yield=d >> lab2_list.csv
./lab2_list --threads=4 --iterations=32 --yield=d >> lab2_list.csv
./lab2_list --threads=8 --iterations=1 --yield=d >> lab2_list.csv
./lab2_list --threads=8 --iterations=2 --yield=d >> lab2_list.csv
./lab2_list --threads=8 --iterations=4 --yield=d >> lab2_list.csv
./lab2_list --threads=8 --iterations=8 --yield=d >> lab2_list.csv
./lab2_list --threads=8 --iterations=16 --yield=d >> lab2_list.csv
./lab2_list --threads=8 --iterations=32 --yield=d >> lab2_list.csv
./lab2_list --threads=12 --iterations=1 --yield=d >> lab2_list.csv
./lab2_list --threads=12 --iterations=2 --yield=d >> lab2_list.csv
./lab2_list --threads=12 --iterations=4 --yield=d >> lab2_list.csv
./lab2_list --threads=12 --iterations=8 --yield=d >> lab2_list.csv
./lab2_list --threads=12 --iterations=16 --yield=d >> lab2_list.csv
./lab2_list --threads=12 --iterations=32 --yield=d >> lab2_list.csv

./lab2_list --threads=2 --iterations=1 --yield=il >> lab2_list.csv
./lab2_list --threads=2 --iterations=2 --yield=il >> lab2_list.csv
./lab2_list --threads=2 --iterations=4 --yield=il >> lab2_list.csv
./lab2_list --threads=2 --iterations=8 --yield=il >> lab2_list.csv
./lab2_list --threads=2 --iterations=16 --yield=il >> lab2_list.csv
./lab2_list --threads=2 --iterations=32 --yield=il >> lab2_list.csv
./lab2_list --threads=4 --iterations=1 --yield=il >> lab2_list.csv
./lab2_list --threads=4 --iterations=2 --yield=il >> lab2_list.csv
./lab2_list --threads=4 --iterations=4 --yield=il >> lab2_list.csv
./lab2_list --threads=4 --iterations=8 --yield=il >> lab2_list.csv
./lab2_list --threads=4 --iterations=16 --yield=il >> lab2_list.csv
./lab2_list --threads=4 --iterations=32 --yield=il >> lab2_list.csv
./lab2_list --threads=8 --iterations=1 --yield=il >> lab2_list.csv
./lab2_list --threads=8 --iterations=2 --yield=il >> lab2_list.csv
./lab2_list --threads=8 --iterations=4 --yield=il >> lab2_list.csv
./lab2_list --threads=8 --iterations=8 --yield=il >> lab2_list.csv
./lab2_list --threads=8 --iterations=16 --yield=il >> lab2_list.csv
./lab2_list --threads=8 --iterations=32 --yield=il >> lab2_list.csv
./lab2_list --threads=12 --iterations=1 --yield=il >> lab2_list.csv
./lab2_list --threads=12 --iterations=2 --yield=il >> lab2_list.csv
./lab2_list --threads=12 --iterations=4 --yield=il >> lab2_list.csv
./lab2_list --threads=12 --iterations=8 --yield=il >> lab2_list.csv
./lab2_list --threads=12 --iterations=16 --yield=il >> lab2_list.csv
./lab2_list --threads=12 --iterations=32 --yield=il >> lab2_list.csv

./lab2_list --threads=2 --iterations=1 --yield=dl >> lab2_list.csv
./lab2_list --threads=2 --iterations=2 --yield=dl >> lab2_list.csv
./lab2_list --threads=2 --iterations=4 --yield=dl >> lab2_list.csv
./lab2_list --threads=2 --iterations=8 --yield=dl >> lab2_list.csv
./lab2_list --threads=2 --iterations=16 --yield=dl >> lab2_list.csv
./lab2_list --threads=2 --iterations=32 --yield=dl >> lab2_list.csv
./lab2_list --threads=4 --iterations=1 --yield=dl >> lab2_list.csv
./lab2_list --threads=4 --iterations=2 --yield=dl >> lab2_list.csv
./lab2_list --threads=4 --iterations=4 --yield=dl >> lab2_list.csv
./lab2_list --threads=4 --iterations=8 --yield=dl >> lab2_list.csv
./lab2_list --threads=4 --iterations=16 --yield=dl >> lab2_list.csv
./lab2_list --threads=4 --iterations=32 --yield=dl >> lab2_list.csv
./lab2_list --threads=8 --iterations=1 --yield=dl >> lab2_list.csv
./lab2_list --threads=8 --iterations=2 --yield=dl >> lab2_list.csv
./lab2_list --threads=8 --iterations=4 --yield=dl >> lab2_list.csv
./lab2_list --threads=8 --iterations=8 --yield=dl >> lab2_list.csv
./lab2_list --threads=8 --iterations=16 --yield=dl >> lab2_list.csv
./lab2_list --threads=8 --iterations=32 --yield=dl >> lab2_list.csv
./lab2_list --threads=12 --iterations=1 --yield=dl >> lab2_list.csv
./lab2_list --threads=12 --iterations=2 --yield=dl >> lab2_list.csv
./lab2_list --threads=12 --iterations=4 --yield=dl >> lab2_list.csv
./lab2_list --threads=12 --iterations=8 --yield=dl >> lab2_list.csv
./lab2_list --threads=12 --iterations=16 --yield=dl >> lab2_list.csv
./lab2_list --threads=12 --iterations=32 --yield=dl >> lab2_list.csv

# STEP 3
