all: context context_thread

context: context.c
	$(CC) -g context.c -o context -lm
	objdump -D context > context.dump

bandwidth: bandwidth.c
	$(CC) -g bandwidth.c -o bandwidth -lm
	objdump -D bandwidth > bandwidth.dump

context_thread: context_thread.c
	$(CC) -g context.c -pthread -o context_thread
	objdump -D context_thread > context_thread.dump
clean:
	rm -f context context_thread measure syscall procedure_call ram_access_time page_fault bandwidth *.dump
