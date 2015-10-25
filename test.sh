#!/bin/bash
#(cd src; make clean; make)
#time ./run.sh ../../outputDir-1k ../1k-queries.txt  > RES
time ./run.sh ../outputDir-10k ../10k-queries.txt > RES 
