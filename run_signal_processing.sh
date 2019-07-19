#!/bin/bash
rm *_HIHIHILB.csv
rm *_hilb.csv
time ./2_hilbertize.py comp0.txt*.csv --window 600
g++ 3_focus.cpp -o 3_focus -lm -O3
time ./3_focus comp0.txt setup_comp.txt
rm *.png
time ./4_sectorize.py phased*.csv -o sect

