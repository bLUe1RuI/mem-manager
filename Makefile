test : test.o mem_manager.o
	cc -o test test.o mem_manager.o

test.o : test.c mem_manager.h
	cc -c -I/usr/include test.c
mem_manager.o : mem_manager.c mem_manager.h
	cc -c -I/usr/include mem_manager.c

clean :
	rm test test.o mem_manager.o
