all: context context_thread bandwidth measurement_overhead page_fault procedure_call ram_access_time syscall

context: ./src/context.c
	$(CC) -g ./src/context.c -o ./obj/context -lm
	objdump -D ./obj/context > ./dump/context.dump

bandwidth: ./src/bandwidth.c
	$(CC) -g ./src/bandwidth.c -o ./obj/bandwidth -lm
	objdump -D ./obj/bandwidth > ./dump/bandwidth.dump

context_thread: ./src/context_thread.c
	$(CC) -g ./src/context_thread.c -pthread -o ./obj/context_thread -lm
	objdump -D ./obj/context_thread > ./dump/context_thread.dump

measurement_overhead: ./src/measurement_overhead.c
	$(CC) -g -O0 ./src/measurement_overhead.c -o ./obj/measurement_overhead -lm
	objdump -D ./obj/measurement_overhead > ./dump/measurement_overhead.dump

page_fault: ./src/page_fault.c
	$(CC) -g -O0 ./src/page_fault.c -o ./obj/page_fault
	objdump -D ./obj/page_fault > ./dump/page_fault.dump

procedure_call: ./src/procedure_call.c
	$(CC) -g -O0 ./src/procedure_call0.c -o ./obj/procedure_call0
	$(CC) -g -O0 ./src/procedure_call1.c -o ./obj/procedure_call1
	$(CC) -g -O0 ./src/procedure_call2.c -o ./obj/procedure_call2
	$(CC) -g -O0 ./src/procedure_call3.c -o ./obj/procedure_call3
	$(CC) -g -O0 ./src/procedure_call4.c -o ./obj/procedure_call4
	$(CC) -g -O0 ./src/procedure_call5.c -o ./obj/procedure_call5
	$(CC) -g -O0 ./src/procedure_call6.c -o ./obj/procedure_call6
	$(CC) -g -O0 ./src/procedure_call7.c -o ./obj/procedure_call7
	objdump -D ./obj/procedure_call > ./dump/procedure_call.dump

ram_access_time: ./src/ram_access_time.c
	$(CC) -g -O0 ./src/ram_access_time.c -o ./obj/ram_access_time
	objdump -D ./obj/ram_access_time > ./dump/ram_access_time.dump

syscall: ./src/syscall.c
	$(CC) -g -O0 ./src/syscall.c -o ./obj/syscall
	objdump -D ./obj/syscall > ./dump/syscall.dump


clean:
	rm -f ./obj/* ./dump/*
