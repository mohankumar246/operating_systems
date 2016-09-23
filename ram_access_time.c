#ifndef RAM_ACCESS_TIME_C
#define RAM_ACCESS_TIME_C
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sched.h>
//#include <preempt.h>
#include <sys/syscall.h>
#include <time.h>
//#include <sys/cachectl.h>

#define MAX_SIZE 16777216 // 8MB
//#define MAX_SIZE 16384  // 32KB

volatile char test_array[MAX_SIZE];
int main(int agrc, char *argv[])
{
	unsigned cycles_high0; 
	unsigned cycles_low0; 
	unsigned cycles_high1; 
	unsigned cycles_low1; 
	uint64_t start,end;

	char test;
	register uint64_t i;
	uint64_t size, stride;
	uint64_t sum;
	//int result;

	//cpu_set_t mask;
	////sched::CPU_ZERO(&mask);
	//CPU_SET(0,&mask);
	//result = sched_setaffinity(0,sizeof(mask),&mask);
	sum = 0;


	//int w = cacheflush();  // correct it 

	//test_array = (char*) malloc(2048*sizeof(char));
	//initialize
	//for(i = 0; i<32768;i++)
	//{
	//	test_array[i] = 0;
	//}

	for(stride = 1; stride<=64; stride = stride*2) 
	{
		printf("\n--------------------stride = %d----------------------\n",stride);
		for(size = 128; size<=MAX_SIZE; size = size*2)
		{
			for(i = 0; i< size; i = i + stride)
			{
				test_array[i] = 'a';
			}
			for(i = 0; i< size; i = i + stride)
			{
				asm volatile ("cpuid\n\t"
			  						"rdtsc\n\t"
			  						"mov %%edx, %0\n\t"
			  						"mov %%eax, %1\n\t"
			  						: "=r" (cycles_high0), "=r" (cycles_low0)
			  						:: "%rax", "%rbx", "%rcx", "%rdx");

				test_array[i] = 'b';

				asm volatile ("rdtscp\n\t"
		  	  						"mov %%edx, %0\n\t"
		  	  						"mov %%eax, %1\n\t"
		  	  						"cpuid\n\t"
		  	  						: "=r" (cycles_high1), "=r" (cycles_low1)
		  	  						:: "%rax", "%rbx", "%rcx", "%rdx");
				start = ( ((uint64_t)cycles_high0 << 32) | cycles_low0 ); 
   			end = ( ((uint64_t)cycles_high1 << 32) | cycles_low1 ); 
				//printf("size = %d writing i=%d in time = %llu clock cycles\n",size, i,(end-start));
				sum = sum + (end-start);

			}
			sum = ((sum*stride) / size);
			printf("size = %d in avg time = %llu clock cycles\n",size,sum);
			sum =0;


		}
	}

	//while(test_array < (test_array+100))
	//for(i = 0; i<10;i++)
	//{
	//	//preempt_disable();
	//	asm volatile ("cpuid\n\t"
	//		  "rdtsc\n\t"
	//		  "mov %%edx, %0\n\t"
	//		  "mov %%eax, %1\n\t"
	//		  : "=r" (cycles_high0), "=r" (cycles_low0)
	//		  :: "%rax", "%rbx", "%rcx", "%rdx");
	//	
	//	test_array[0] = 'a';
   //	
	//	asm volatile ("rdtscp\n\t"
	//	  	  "mov %%edx, %0\n\t"
	//	  	  "mov %%eax, %1\n\t"
	//	  	  "cpuid\n\t"
	//	  	  : "=r" (cycles_high1), "=r" (cycles_low1)
	//	  	  :: "%rax", "%rbx", "%rcx", "%rdx");

	//	//preempt_enable();
	//	start = ( ((uint64_t)cycles_high0 << 32) | cycles_low0 ); 
   //	end = ( ((uint64_t)cycles_high1 << 32) | cycles_low1 ); 

	//	//printf("high0 = %d, high1 = %d, low0 = %d, low1 = %d\n",cycles_high0,cycles_high1,cycles_low0,cycles_low1);
	//	printf("write test array %x time = %llu clock cycles\n",test_array,(end-start));


	//	asm volatile ("cpuid\n\t"
	//		  "rdtsc\n\t"
	//		  "mov %%edx, %0\n\t"
	//		  "mov %%eax, %1\n\t"
	//		  : "=r" (cycles_high0), "=r" (cycles_low0)
	//		  :: "%rax", "%rbx", "%rcx", "%rdx");

	//	test = test_array[0];
	//	
	//	asm volatile ("rdtscp\n\t"
	//	  	  "mov %%edx, %0\n\t"
	//	  	  "mov %%eax, %1\n\t"
	//	  	  "cpuid\n\t"
	//	  	  : "=r" (cycles_high1), "=r" (cycles_low1)
	//	  	  :: "%rax", "%rbx", "%rcx", "%rdx");


	//	//preempt_enable();
	//	start = ( ((uint64_t)cycles_high0 << 32) | cycles_low0 ); 
   //	end = ( ((uint64_t)cycles_high1 << 32) | cycles_low1 ); 

	//	//printf("high0 = %d, high1 = %d, low0 = %d, low1 = %d\n",cycles_high0,cycles_high1,cycles_low0,cycles_low1);
	//	printf("read test array %x time = %llu clock cycles\n",test_array,(end-start));

	//	//test_array++;

	//}


   //free(test_array);



}

#endif
