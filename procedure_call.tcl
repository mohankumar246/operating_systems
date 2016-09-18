rm -f procedure_call
rm -f procedure_call.dump
gcc -g -O3 procedure_call.c -o procedure_call
objdump -D procedure_call > procedure_call.dump
