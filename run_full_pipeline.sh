#!/bin/bash
rm *.png *.csv
g++ 1_propagate.cpp -o 1_propagate -lm -O3
time ./1_propagate baseline.txt setup_base.txt
# loading 2_hilbertize.py and loading final files to the folders:
time ./2_hilbertize.py baseline.txt*.csv --window 10000 
g++ 3_focus.cpp -o 3_focus -lm -O3
time ./3_focus baseline.txt setup_base.txt
time ./4_sectorize.py phased*.csv -o sect

