all:
	gcc -fPIC -c CollatzLibrary.c
	gcc -shared CollatzLibrary.o -o libCollatzLibrary.so 
	gcc dynamic.c -ldl -o dynamic.out

clean:
	rm *.so
	rm *.o
