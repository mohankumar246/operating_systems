#ifndef RAM_ACCESS_TIME_C
#define RAM_ACCESS_TIME_C
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sched.h>
#include <sys/syscall.h>
#include <time.h>

#define MB 1024*1024
#define MAX_SIZE_MB 128 
#define MAX_SIZE MAX_SIZE_MB*MB // 128MB
#define MAX_STRIDE 2048*4
#define CLOCK 1700

#define O11 n = (volatile uint64_t**)*n;
#define T22 O11 O11
#define F44 T22 T22 
#define EE8 F44 F44
#define OS6 EE8 EE8
#define TH2 OS6 OS6
#define S64 TH2 TH2
#define ONE_28 S64 S64
int main(int agrc, char *argv[])
{
	unsigned cycles_high0; 
	unsigned cycles_low0; 
	unsigned cycles_high1; 
	unsigned cycles_low1; 
	uint64_t start,end;
	register uint64_t count=0;
	volatile uint64_t  **n;
	uint64_t **test_array;
	int array_size;
	//Icache warmup
	asm volatile ("cpuid\n\t"
		  "rdtsc\n\t"
		  "mov %%edx, %0\n\t"
		  "mov %%eax, %1\n\t"
		  : "=r" (cycles_high0), "=r" (cycles_low0)
		  :: "%rax", "%rbx", "%rcx", "%rdx");

//	printf("%lu %lu\n",sizeof(uint64_t*),sizeof(uint64_t));
	for(uint64_t size = 64*1024; size< MAX_SIZE; size *=2)
	{
		test_array = malloc(size);
		array_size = size/sizeof(uint64_t);
		//printf("\n---------- array size = %lu ----------\n",array_size);
		printf("\n---------------------\n");
		for(uint64_t stride = 1; stride <= MAX_STRIDE; stride = stride*2) 
		{
			for(uint64_t index=0; index < array_size; index++)
			{
				test_array[index] = (uint64_t*)&test_array[(index+stride)%array_size];
			}				

			//printf("%llu %llu %llu %llu \n",&test_array[0],&test_array[1],test_array[0],test_array[1]);

			n = (volatile uint64_t **)&test_array[0];		
			
			//printf("%llu %llu %llu\n",n,*n,(int**)*n);

			//printf("\n----------stride = %lu ----------\n",stride);
			asm volatile ("cpuid\n\t"
		  		      "rdtsc\n\t"
		  		      "mov %%edx, %0\n\t"
		  		      "mov %%eax, %1\n\t"
		  		      : "=r" (cycles_high0), "=r" (cycles_low0)
		  		      :: "%rax", "%rbx", "%rcx", "%rdx");
				
			for(int iter=0;iter<5;iter++)
			{
				n = (volatile uint64_t **)&test_array[0];		
				for(count = 0; count < (array_size/stride);count++)
				{	
					ONE_28	
				}
			}
			asm volatile ("rdtscp\n\t"
	  	  		      "mov %%edx, %0\n\t"
	  	  		      "mov %%eax, %1\n\t"
	  	  		      "cpuid\n\t"
	  	  		      : "=r" (cycles_high1), "=r" (cycles_low1)
	  	  		      :: "%rax", "%rbx", "%rcx", "%rdx");

			start = (((uint64_t)cycles_high0 << 32) | cycles_low0 ); 
			end   = (((uint64_t)cycles_high1 << 32) | cycles_low1 ); 
			
			float total = end-start;

			if(total < 0)
				printf("error");
			//printf("size = %d writing i=%d in time = %llu clock cycles\n",size, i,(end-start));
			printf("Array size = %f MB, Stride %lu, Time = %f cycles, %f micro s\n",((float)size/(MB)), stride,
			(total*stride)/(5*((float)(array_size))*128),(total*stride)/(5*CLOCK*((float)(array_size*128))));
		}
		free(test_array);
	}
}

#endif
