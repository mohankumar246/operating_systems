rm -f syscall
rm -f syscall.dump
gcc -g -O0 syscall.c -o syscall
objdump -D syscall > syscall.dump
