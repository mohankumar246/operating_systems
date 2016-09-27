CC=gcc
SRC=./src
OBJ=./obj
DUMP=./dump

all: context context_thread bandwidth measurement_overhead page_fault procedure_call ram_access_time syscall


context: $(SRC)/context.c
	$(CC) -g -O0 $(SRC)/context.c -o $(OBJ)/context -lm
	objdump -D $(OBJ)/context > $(DUMP)/context.dump

bandwidth: $(SRC)/bandwidth.c
	$(CC) -g -O0 $(SRC)/bandwidth.c -o $(OBJ)/bandwidth -lm
	objdump -D $(OBJ)/bandwidth > $(DUMP)/bandwidth.dump

context_thread: $(SRC)/context_thread.c
	$(CC) -g -O0 $(SRC)/context_thread.c -pthread -o $(OBJ)/context_thread -lm
	objdump -D $(OBJ)/context_thread > $(DUMP)/context_thread.dump

measurement_overhead: $(SRC)/measurement_overhead.c
	$(CC) -g -O0 $(SRC)/measurement_overhead.c -o $(OBJ)/measurement_overhead -lm
	objdump -D $(OBJ)/measurement_overhead > $(DUMP)/measurement_overhead.dump

page_fault: $(SRC)/page_fault.c
	$(CC) -g -O0 $(SRC)/page_fault.c -o $(OBJ)/page_fault
	objdump -D $(OBJ)/page_fault > $(DUMP)/page_fault.dump

procedure_call: $(SRC)/procedure_call.c
	$(CC) -g -O0 ./src/procedure_call0.c -o $(OBJ)/procedure_call0
	$(CC) -g -O0 ./src/procedure_call1.c -o $(OBJ)/procedure_call1
	$(CC) -g -O0 ./src/procedure_call2.c -o $(OBJ)/procedure_call2
	$(CC) -g -O0 ./src/procedure_call3.c -o $(OBJ)/procedure_call3
	$(CC) -g -O0 ./src/procedure_call4.c -o $(OBJ)/procedure_call4
	$(CC) -g -O0 ./src/procedure_call5.c -o $(OBJ)/procedure_call5
	$(CC) -g -O0 ./src/procedure_call6.c -o $(OBJ)/procedure_call6
	$(CC) -g -O0 ./src/procedure_call7.c -o $(OBJ)/procedure_call7
	$(CC) -g -O0 $(SRC)/procedure_call.c -o $(OBJ)/procedure_call
	objdump -D $(OBJ)/procedure_call > $(DUMP)/procedure_call.dump

ram_access_time: $(SRC)/ram_access_time.c
	$(CC) -g -O0 $(SRC)/ram_access_time.c -o $(OBJ)/ram_access_time
	objdump -D $(OBJ)/ram_access_time > $(DUMP)/ram_access_time.dump

syscall: $(SRC)/syscall.c
	$(CC) -g -O0 $(SRC)/syscall.c -o $(OBJ)/syscall
	objdump -D $(OBJ)/syscall > $(DUMP)/syscall.dump


clean:
	rm -f $(OBJ)/* $(DUMP)/*
