#!/bin/bash

g++ butterfly_csv.cpp -o bdcsv -lm -O3
cp p0.5.txt obstacles.txt
time ./bdcsv
echo 0.5 done
./hilbertize.py out.csv -o prism_0.5 --freq_min 150 --freq_max 550
cp p0.6.txt obstacles.txt
time ./bdcsv
echo 0.6 done
./hilbertize.py out.csv -o prism_0.6 --freq_min 150 --freq_max 550
cp p0.7.txt obstacles.txt
time ./bdcsv
echo 0.7 done
./hilbertize.py out.csv -o prism_0.7 --freq_min 150 --freq_max 550
cp p0.8.txt obstacles.txt
time ./bdcsv
echo 0.8 done
./hilbertize.py out.csv -o prism_0.8 --freq_min 150 --freq_max 550
cp p0.9.txt obstacles.txt
time ./bdcsv
echo 0.9 done
./hilbertize.py out.csv -o prism_0.9 --freq_min 150 --freq_max 550
cp p1.0.txt obstacles.txt
time ./bdcsv
echo 1.0 done
./hilbertize.py out.csv -o prism_1.0 --freq_min 150 --freq_max 550
cp p1.1.txt obstacles.txt
time ./bdcsv
echo 1.1 done
./hilbertize.py out.csv -o prism_1.1 --freq_min 150 --freq_max 550
./csv2png.py *.csv
cp p1.2.txt obstacles.txt
time ./bdcsv
echo 1.2 done
./hilbertize.py out.csv -o prism_1.2 --freq_min 150 --freq_max 550
cp p1.3.txt obstacles.txt
time ./bdcsv
echo 1.3 done
./hilbertize.py out.csv -o prism_1.3 --freq_min 150 --freq_max 550
cp p1.4.txt obstacles.txt
time ./bdcsv
echo 1.4 done
./hilbertize.py out.csv -o prism_1.4 --freq_min 150 --freq_max 550
cp p1.5.txt obstacles.txt
time ./bdcsv
echo 1.5 done
./hilbertize.py out.csv -o prism_1.5 --freq_min 150 --freq_max 550

