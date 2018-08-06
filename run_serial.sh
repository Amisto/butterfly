#!/bin/bash

g++ butterfly_serial.cpp -o bds -lm -O3
time ./bds fin_0.8_3.txt setup_3.txt
./hilbertize_serial.py phased_fin_0.8_3.txt_shift_*.csv -o serial_3 --freq_min 150 --freq_max 550
time ./bds fin_0.8_5.txt setup_5.txt
./hilbertize_serial.py phased_fin_0.8_5.txt_shift_*.csv -o serial_5 --freq_min 150 --freq_max 550
