ifeq ($(DEBUG),true)
	CC = gcc -g
else
	CC = gcc
endif

all: dirStat.o
	$(CC) -o dirStat dirStat.o

dirStat.o: dirStat.c
	$(CC) -c dirStat.c

run:
	./dirStat .