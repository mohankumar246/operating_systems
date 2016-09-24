rm -f page_fault
rm -f page_fault.dump
gcc -g -O0 page_fault.c -o page_fault
objdump -D page_fault > page_fault.dump
