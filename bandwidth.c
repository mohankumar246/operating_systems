#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sched.h>
#include <sys/syscall.h>
#include <time.h>
#include <string.h>
#include <memory.h>
#define SIZE 50*1024*1024
#define SPEED 1700000000
#define SPEEDMhz 1700

int main(int agrc, char *argv[])
{

	register char *source = malloc(SIZE);
	register char *dest   = malloc(SIZE);
	unsigned start_h,start_l,end_h,end_l;
	uint64_t time_c;
	
	for(int i=0;i< 10;i++)
	{
	asm volatile ("CPUID\n\t"
	             "RDTSC\n\t"
                     "mov %%edx, %0\n\t"
                     "mov %%eax, %1\n\t": "=r" (start_h), "=r" (start_l)::
                     "%rax", "%rbx", "%rcx", "%rdx");

	memset(source,0,SIZE);

        asm volatile ("rdtscp\n\t"
                     "mov %%edx, %0\n\t"
                     "mov %%eax, %1\n\t"
                     "cpuid\n\t"
                     : "=r" (end_h), "=r" (end_l)
                     :: "%rax", "%rbx", "%rcx", "%rdx");

	time_c = (((uint64_t)end_h << 32) | end_l) - (((uint64_t)start_h << 32)|start_l) ;

	printf("Time for memset %f, BW %f MB/s\n",(float)time_c/SPEED, ((float)50 * SPEED)/time_c);	 

	asm volatile ("CPUID\n\t"
	             "RDTSC\n\t"
                     "mov %%edx, %0\n\t"
                     "mov %%eax, %1\n\t": "=r" (start_h), "=r" (start_l)::
                     "%rax", "%rbx", "%rcx", "%rdx");

	memcpy(dest,source,SIZE);

        asm volatile ("rdtscp\n\t"
                     "mov %%edx, %0\n\t"
                     "mov %%eax, %1\n\t"
                     "cpuid\n\t"
                     : "=r" (end_h), "=r" (end_l)
                     :: "%rax", "%rbx", "%rcx", "%rdx");

	time_c = (((uint64_t)end_h << 32) | end_l) - (((uint64_t)start_h << 32)|start_l) ;

	printf("Time for memcpy %f, BW %f MB/s\n",(float)time_c/SPEED, ((float)50 * SPEED)/time_c);	 

	}
}
