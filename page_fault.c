// page fault time
#ifndef PAGE_FAULT_C
#define PAGE_FAULT_C
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sched.h>
//#include <preempt.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
//#include <sys/cachectl.h>

int main(int agrc, char *argv[])
{
	unsigned cycles_high0; 
	unsigned cycles_low0; 
	unsigned cycles_high1; 
	unsigned cycles_low1; 
	uint64_t start,end,diff;
	off_t offset;
	struct stat sb;
	size_t len;
	int fd;
	char *op,test;

	uint64_t i;

	//Icache warmup
	asm volatile ("cpuid\n\t"
		  "rdtsc\n\t"
		  "mov %%edx, %0\n\t"
		  "mov %%eax, %1\n\t"
		  : "=r" (cycles_high0), "=r" (cycles_low0)
		  :: "%rax", "%rbx", "%rcx", "%rdx");

	fd = open(argv[1],O_RDONLY);
	if(fd == -1)
	{
		printf("problem in opening\n");
		return 1;
	}

	if(stat(argv[1],&sb)==-1)
	{
		printf("statmapping problem\n");
		return 1;
	}


	printf("size = %llu\n", sb.st_size);
	asm volatile ("cpuid\n\t"
						"rdtsc\n\t"
						"mov %%edx, %0\n\t"
						"mov %%eax, %1\n\t"
						: "=r" (cycles_high0), "=r" (cycles_low0)
						:: "%rax", "%rbx", "%rcx", "%rdx");
	op = mmap(NULL,sb.st_size,PROT_READ,MAP_SHARED,fd,0);
	asm volatile ("rdtscp\n\t"
						"mov %%edx, %0\n\t"
						"mov %%eax, %1\n\t"
						"cpuid\n\t"
						: "=r" (cycles_high1), "=r" (cycles_low1)
						:: "%rax", "%rbx", "%rcx", "%rdx");
	start = ( ((uint64_t)cycles_high0 << 32) | cycles_low0 ); 
   end = ( ((uint64_t)cycles_high1 << 32) | cycles_low1 ); 

	printf("cycles mmap = %llu \n",(end-start));


	if(op == MAP_FAILED)
	{
		printf("mmap failed\n");
		return 1;
	}

	// 1st access - should be a page fault
	for(i=0;i<sb.st_size;i++)
	{
		asm volatile ("cpuid\n\t"
							"rdtsc\n\t"
							"mov %%edx, %0\n\t"
							"mov %%eax, %1\n\t"
							: "=r" (cycles_high0), "=r" (cycles_low0)
							:: "%rax", "%rbx", "%rcx", "%rdx");

		test= *op;

		
		asm volatile ("rdtscp\n\t"
							"mov %%edx, %0\n\t"
							"mov %%eax, %1\n\t"
							"cpuid\n\t"
							: "=r" (cycles_high1), "=r" (cycles_low1)
							:: "%rax", "%rbx", "%rcx", "%rdx");
		start = ( ((uint64_t)cycles_high0 << 32) | cycles_low0 ); 
   	end = ( ((uint64_t)cycles_high1 << 32) | cycles_low1 ); 

		//printf("i = %d test = %c cycles page fault = %llu \n",i,test,(end-start));
		if((end-start)>100)
			printf("FOUND %d %llu\n",i,(end-start));

		// accessing next byte - should be a page hit
		op++;
	}
	//asm volatile ("cpuid\n\t"
	//					"rdtsc\n\t"
	//					"mov %%edx, %0\n\t"
	//					"mov %%eax, %1\n\t"
	//					: "=r" (cycles_high0), "=r" (cycles_low0)
	//					:: "%rax", "%rbx", "%rcx", "%rdx");

	//test= *op;

	//
	//asm volatile ("rdtscp\n\t"
	//					"mov %%edx, %0\n\t"
	//					"mov %%eax, %1\n\t"
	//					"cpuid\n\t"
	//					: "=r" (cycles_high1), "=r" (cycles_low1)
	//					:: "%rax", "%rbx", "%rcx", "%rdx");
	//start = ( ((uint64_t)cycles_high0 << 32) | cycles_low0 ); 
   //end = ( ((uint64_t)cycles_high1 << 32) | cycles_low1 ); 

	//printf("test = %c cycles for page hit = %llu \n",test, (end-start));	
	
	
	
	
	
	printf("Reached till here safely now exiting\n",test);

	return 0;

}
#endif
