CC=gcc

%.o:%.c
	$(CC) -c -o $@ $^

clean:
	rm -f *.o
