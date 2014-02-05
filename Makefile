warmup2: warmup2.o my402list.o
	gcc -pthread -lpthread  -o warmup2 -g warmup2.o my402list.o -lm

warmup2.o: warmup2.c my402list.h
	gcc   -g -c -Wall warmup2.c

my402list.o: my402list.c my402list.h
	gcc -g -c -Wall my402list.c

clean:
	rm -f *.o warmup2
