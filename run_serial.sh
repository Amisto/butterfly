#!/bin/bash

g++ butterfly_serial.cpp -o bds -lm -O3
time ./bds p_serial.txt
./hilbertize_serial.py phased*.csv -o serial --freq_min 150 --freq_max 550
cp *.png ./md
time ./bds p_serial_r.txt
./hilbertize_serial.py phased*.csv -o serial --freq_min 150 --freq_max 550
cp *.png ./mr

