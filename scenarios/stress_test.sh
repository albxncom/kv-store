#!/bin/bash
# @Author: albin
# @Date:   2020-05-26 19:09:23
# @Last Modified by:   albin
# @Last Modified time: 2020-05-26 19:58:51

# start 1000 connections

connections=0

# start first 9 X 10 = 90
for i in {0..9}
do
	for g in {1..10}
	do
		./scenario3 800"$i" 10000 20 500 &
		sleep .01
		((connections++))
	done
done


# continue with 10..99 = 89 X 10 = 890
for j in {10..99}
do
	for g in {1..10}
	do
		./scenario3 80"$j" 10000 20 500 &
		sleep .01
		((connections++))
	done
done

echo "$connections"