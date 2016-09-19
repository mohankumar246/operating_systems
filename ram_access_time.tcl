rm -f ram_access_time
rm -f ram_access_time.dump
gcc -g -O0 ram_access_time.c -o ram_access_time
objdump -D ram_access_time > ram_access_time.dump
