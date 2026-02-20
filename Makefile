# Makefile for Battleships game

all:
	gcc -Wall -Wextra -std=c99 battleships.c -o battleships

clean:
	rm -f battleships

.PHONY: all clean
