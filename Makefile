ifeq ($(DEBUG),true)
	CC = gcc -g
else
	CC = gcc
endif

all: dirStat.o
	$(CC) -o dirStat dirStat.o -lm

dirStat.o: dirStat.c
	$(CC) -c dirStat.c -lm

run:
	./dirStat . -r