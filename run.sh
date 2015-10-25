#!/bin/bash

datapath=$1
querypath=$2

# 1. split file
./src/splitfile $querypath

# 2. execute different type of query separately
time ./src/query1 query1.in $datapath & # Q1
time ./src/query2 query2.in $datapath & # Q2
time ./src/query3 $datapath query3.in & # Q3
time ./src/query4 query4.in $datapath & # Q4
wait

# 3. finish task by merging types of answers
./src/mergeans

