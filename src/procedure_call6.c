// 1 argument
#ifndef PROCEDURE_CALL_C
#define PROCEDURE_CALL_C
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <sched.h>
#include <sys/syscall.h>
#include <time.h>

unsigned cycles_high0; 
unsigned cycles_low0; 
unsigned cycles_high1; 
unsigned cycles_low1; 
uint64_t start,end;
uint64_t sum,avg;
uint64_t call_overhead,ret_overhead;

void call_func(int a1, int a2, int a3, int a4, int a5, int a6)
{
   asm volatile ("rdtscp\n\t"
	    "mov %%edx, %0\n\t"
	    "mov %%eax, %1\n\t"
	    "cpuid\n\t"
	    : "=r" (cycles_high1), "=r" (cycles_low1)
	    :: "%rax", "%rbx", "%rcx", "%rdx");
	start = ( ((uint64_t)cycles_high0 << 32) | cycles_low0 ); 
   end = ( ((uint64_t)cycles_high1 << 32) | cycles_low1 );

	call_overhead = call_overhead + (end-start);


	float mean;
	//printf("Lets take mean of this numbers\n");

	mean = (a1+a2+a3+a4+a5+a6)/4;
	printf("mean = %f\n",mean);


	asm volatile ("cpuid\n\t"
		  "rdtsc\n\t"
		  "mov %%edx, %0\n\t"
		  "mov %%eax, %1\n\t"
		  : "=r" (cycles_high0), "=r" (cycles_low0)
		  :: "%rax", "%rbx", "%rcx", "%rdx");
}
int main(int agrc, char *argv[])
{
	int i;
	sum = 0;
	call_overhead = 0;
	ret_overhead = 0;

	//preempt_disable();
	for(i =0;i<100; i++)
	{
		asm volatile ("cpuid\n\t"
			  "rdtsc\n\t"
			  "mov %%edx, %0\n\t"
			  "mov %%eax, %1\n\t"
			  : "=r" (cycles_high0), "=r" (cycles_low0)
			  :: "%rax", "%rbx", "%rcx", "%rdx");

	  call_func(2,3,4,5,6,7);
		
     asm volatile ("rdtscp\n\t"
	      "mov %%edx, %0\n\t"
	      "mov %%eax, %1\n\t"
	      "cpuid\n\t"
	      : "=r" (cycles_high1), "=r" (cycles_low1)
	      :: "%rax", "%rbx", "%rcx", "%rdx");


		//preempt_enable();
		start = ( ((uint64_t)cycles_high0 << 32) | cycles_low0 ); 
   	end = ( ((uint64_t)cycles_high1 << 32) | cycles_low1 ); 

		//printf("high0 = %d, high1 = %d, low0 = %d, low1 = %d\n",cycles_high0,cycles_high1,cycles_low0,cycles_low1);
		//printf("call4 overhead = %llu clock cycles\n",(end-start));
		ret_overhead = ret_overhead + (end - start);
	}
	call_overhead = (call_overhead/100);
	ret_overhead = (ret_overhead/100);

	printf("call overhead = %d, ret overhead = %d\n",call_overhead,ret_overhead);
	printf("Average measurement overhead time for call = %llu clock cycles \n",(call_overhead+ret_overhead));


}

#endif

