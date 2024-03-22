all:
	gcc -fPIC -c CollatzLibrary.c
	gcc -shared CollatzLibrary.o -o libCollatzLibrary.so 
	gcc shared.c -L. -lCollatzLibrary -Wl,-rpath=. -o shared.out

clean:
	rm *.so
	rm *.o
