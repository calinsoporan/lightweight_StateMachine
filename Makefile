all:
	$(CC) -fPIC -g -c -Wall sm.c
	$(CC) -shared -o libsm.so sm.o

clean:
	rm -rf *.o
