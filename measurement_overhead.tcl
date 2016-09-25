rm -f measure
rm -f measure.dump
gcc -g -O0 measurement_overhead.c -o measure
objdump -D measure > measure.dump
