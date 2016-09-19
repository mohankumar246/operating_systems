#ifndef RAM_ACCESS_TIME_C
#define RAM_ACCESS_TIME_C
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sched.h>
#include <preempt.h>
#include <sys/syscall.h>
#include <time.h>
#include <asm/cachectl.h>

int main(int agrc, char *argv[])
{
	unsigned cycles_high0; 
	unsigned cycles_low0; 
	unsigned cycles_high1; 
	unsigned cycles_low1; 
	uint64_t start,end;

	char test_array[2048];
	char test;

	int w = cacheflush();  // correct it 

	//test_array = (char*) malloc(2048*sizeof(char));

	//while(test_array < (test_array+100))
	int i;
	for(i = 0; i<10;i++)
	{
		//preempt_disable();
		asm volatile ("cpuid\n\t"
			  "rdtsc\n\t"
			  "mov %%edx, %0\n\t"
			  "mov %%eax, %1\n\t"
			  : "=r" (cycles_high0), "=r" (cycles_low0)
			  :: "%rax", "%rbx", "%rcx", "%rdx");
		
		test_array[0] = 'a';
   	
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
		printf("write test array %d time = %llu clock cycles\n",test_array,(end-start));


		asm volatile ("cpuid\n\t"
			  "rdtsc\n\t"
			  "mov %%edx, %0\n\t"
			  "mov %%eax, %1\n\t"
			  : "=r" (cycles_high0), "=r" (cycles_low0)
			  :: "%rax", "%rbx", "%rcx", "%rdx");

		test = test_array[0];
		
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
		printf("read test array %d time = %llu clock cycles\n",test_array,(end-start));

		//test_array++;

	}


   //free(test_array);



}

#endif
