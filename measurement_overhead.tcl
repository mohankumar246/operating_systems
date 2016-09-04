rm -f measure
rm -f measure.dump
gcc -g -O3 measurement_overhead.c -o measure
objdump -D measure > measure.dump
