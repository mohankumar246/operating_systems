#ifndef MEASUREMENT_OVERHEAD_C
#define MEASUREMENT_OVERHEAD_C
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sched.h>
#include <preempt.h>
#include <sys/syscall.h>
#include <time.h>

int main(int agrc, char *argv[])
{
	unsigned cycles_high0; 
	unsigned cycles_low0; 
	unsigned cycles_high1; 
	unsigned cycles_low1; 
	uint64_t start,end;

	//preempt_disable();
	asm volatile ("cpuid\n\t"
		  "rdtsc\n\t"
		  "mov %%edx, %0\n\t"
		  "mov %%eax, %1\n\t"
		  : "=r" (cycles_high0), "=r" (cycles_low0)
		  :: "%rax", "%rbx", "%rcx", "%rdx");

    /* no code here as we are just looking at timing measurement overhead */

    asm volatile ("rdtscp\n\t"
		  "mov %%edx, %0\n\t"
		  "mov %%eax, %1\n\t"
		  "cpuid\n\t"
		  : "=r" (cycles_high1), "=r" (cycles_low1)
		  :: "%rax", "%rbx", "%rcx", "%rdx");

	//preempt_enable();
	start = ( ((uint64_t)cycles_high0 << 32) | cycles_low0 ); 
   end = ( ((uint64_t)cycles_high1 << 32) | cycles_low1 ); 

	printf("high0 = %d, high1 = %d, low0 = %d, low1 = %d\n",cycles_high0,cycles_high1,cycles_low0,cycles_low1);
	printf("measurement overhead time = %llu clock cycles\n",(end-start));


	// loop overhead measurement
	
	volatile int a;
	uint64_t cycles_without_loop, cycles_with_loop;

	a=0;
	asm volatile ("cpuid\n\t"
		  "rdtsc\n\t"
		  "mov %%edx, %0\n\t"
		  "mov %%eax, %1\n\t"
		  : "=r" (cycles_high0), "=r" (cycles_low0)
		  :: "%rax", "%rbx", "%rcx", "%rdx");

	a++;
	a++;
	a++;
	a++;
	a++;
	a++;
	a++;
	a++;
	a++;
	a++;

	asm volatile ("rdtscp\n\t"
		  "mov %%edx, %0\n\t"
		  "mov %%eax, %1\n\t"
		  "cpuid\n\t"
		  : "=r" (cycles_high1), "=r" (cycles_low1)
		  :: "%rax", "%rbx", "%rcx", "%rdx");

	start = ( ((uint64_t)cycles_high0 << 32) | cycles_low0 ); 
   end = ( ((uint64_t)cycles_high1 << 32) | cycles_low1 ); 

	cycles_without_loop = end-start;

	printf("high0 = %d, high1 = %d, low0 = %d, low1 = %d\n",cycles_high0,cycles_high1,cycles_low0,cycles_low1);
	printf("cycles taken without loop = %llu clock cycles\n",cycles_without_loop);
	
	a=0;
	
	asm volatile ("cpuid\n\t"
		  "rdtsc\n\t"
		  "mov %%edx, %0\n\t"
		  "mov %%eax, %1\n\t"
		  : "=r" (cycles_high0), "=r" (cycles_low0)
		  :: "%rax", "%rbx", "%rcx", "%rdx");

	while(a<10)
		a++;
	
	asm volatile ("rdtscp\n\t"
	  "mov %%edx, %0\n\t"
	  "mov %%eax, %1\n\t"
	  "cpuid\n\t"
	  : "=r" (cycles_high1), "=r" (cycles_low1)
	  :: "%rax", "%rbx", "%rcx", "%rdx");

	start = ( ((uint64_t)cycles_high0 << 32) | cycles_low0 ); 
	end = ( ((uint64_t)cycles_high1 << 32) | cycles_low1 ); 
	
	cycles_with_loop = end-start;
	
	printf("high0 = %d, high1 = %d, low0 = %d, low1 = %d\n",cycles_high0,cycles_high1,cycles_low0,cycles_low1);
	printf("cycles taken without loop = %llu clock cycles\n",cycles_with_loop);

	printf("Thus the LOOP OVERHEAD = %llu clock cycles\n",(cycles_with_loop - cycles_without_loop));


}

#endif
