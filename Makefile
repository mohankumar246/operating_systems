all: context context_thread

context: context.c
	$(CC) -g context.c -o context
	objdump -D context > context.dump

context_thread: context_thread.c
	$(CC) -g context.c -pthread -o context_thread
	objdump -D context_thread > context_thread.dump
clean:
	rm -f context context_thread procedure_call ram_access_time *.dump
