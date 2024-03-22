all:
	gcc -c -o CollatzLibrary.o CollatzLibrary.c
	ar rcs -o libCollatzLibrary.a CollatzLibrary.o
	gcc static.c CollatzLibrary.o -o static.out

clean:
	rm *.a 
	rm *.o
