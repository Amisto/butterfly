#!/bin/bash
g++ 1_propagate.cpp -o 1_propagate -lm -O3
#time ./1_propagate baseline.txt setup_base.txt
# loading 2_hilbertize.py and loading final files to the :
time python3 2_hilbertize.py --window 10000
g++ 3_focus.cpp -o 3_focus -lm -O3
time ./3_focus baseline.txt setup_base.txt
time python3 4_sectorize.py