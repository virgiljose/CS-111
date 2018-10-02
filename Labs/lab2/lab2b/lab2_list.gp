#!/usr/bin/gnuplot


# general plot parameters
set terminal png
set datafile separator ","

# Total Number of Operations per Second, Each Synchronization Method
set title "lab2b_1: Number of Operations per Second, Synchronization Methods"
set xlabel "Threads"
set ylabel "Operations per Second"
set xrange [1:32]
set logscale x 2
set logscale y 10
set output 'lab2b_1.png'

# grep out no-yield, mutex and spin-lock
plot \
     "< grep 'list-none-m,' lab2b_list.csv | grep 1000,1," using ($2):(1000000000/($7)) \
	title 'List Mutex' with linespoints lc rgb 'red', \
     "< grep 'list-none-s,' lab2b_list.csv | grep 1000,1," using ($2):(1000000000/($7)) \
	title 'List Spin-Lock' with linespoints lc rgb 'green'


# Avg wait-for-lock time, avg time per operation
set title "lab2b_2: Mutex: Wait-For-Lock Time, Average Time per Operation"
set xlabel "Threads"
set ylabel "Time per Operation"
set xrange [1:32]
set logscale x 2
set logscale y 10
set output 'lab2b_2.png'
plot \
     "< grep 'list-none-m,' lab2b_list.csv | grep 1000,1," using ($2):($7) \
	title 'time per operation' with linespoints lc rgb 'green', \
     "< grep 'list-none-m,' lab2b_list.csv | grep 1000,1," using ($2):($8) \
	title 'wait for lock time' with linespoints lc rgb 'red'


# Iterations that run without failure, each synchronization method     
set title "Iterations that run without failure"
set logscale x 2
set xrange [1:32]
set logscale y 10
set xlabel "Threads"
set ylabel "Successful Iterations"
set output 'lab2b_3.png'
plot \
    "< grep 'list-id-none' lab2b_list.csv" using ($2):($3) \
	with points lc rgb 'blue' title 'No sync', \
    "< grep 'list-id-m' lab2b_list.csv" using ($2):($3) \
        with points lc rgb 'red' title 'Mutex', \
    "< grep 'list-id-s' lab2b_list.csv" using ($2):($3) \
       	with points lc rgb 'green' title 'Spin Lock'


# Number of Threads vs Throughput: Mutex
set title "Number of threads vs throughput - mutex"
set logscale x 2
set xrange [1:32]
set logscale y 10
set xlabel "Threads"
set ylabel "Throughput"
set output 'lab2b_4.png'
plot \
     "< grep 'list-none-m,' lab2b_list.csv | grep 1000,1," using ($2):(1000000000/$7) \
     	with linespoints lc rgb 'red' title '1 list', \
     "< grep 'list-none-m,' lab2b_list.csv | grep 1000,4," using ($2):(1000000000/$7) \
     	with linespoints lc rgb 'orange' title '4 lists', \
     "< grep 'list-none-m,' lab2b_list.csv | grep 1000,8," using ($2):(1000000000/$7) \
     	with linespoints lc rgb 'green' title '8 lists', \
     "< grep 'list-none-m,' lab2b_list.csv | grep 1000,16," using ($2):(1000000000/$7) \
     	with linespoints lc rgb 'blue' title '16 lists'

# Number of Threads vs Throughput: Spin-Locks
set title "Number of threads vs throughput - spin-locks"
set logscale x 2
set xrange [1:32]
set logscale y 10
set xlabel "Threads"
set ylabel "Throughput"
set output 'lab2b_5.png'
plot \
     "< grep 'list-none-s,' lab2b_list.csv | grep 1000,1," using ($2):(1000000000/$7) \
     	with linespoints lc rgb 'red' title '1 list', \
     "< grep 'list-none-s,' lab2b_list.csv | grep 1000,4," using ($2):(1000000000/$7) \
     	with linespoints lc rgb 'orange' title '4 lists', \
     "< grep 'list-none-s,' lab2b_list.csv | grep 1000,8," using ($2):(1000000000/$7) \
     	with linespoints lc rgb 'green' title '8 lists', \
     "< grep 'list-none-s,' lab2b_list.csv | grep 1000,16," using ($2):(1000000000/$7) \
     	with linespoints lc rgb 'blue' title '16 lists'