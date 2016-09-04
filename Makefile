all: context

context: context.c
	$(CC) -g -O3 context.c -o context
	objdump -D context > context.dump

clean:
	rm -f context 
