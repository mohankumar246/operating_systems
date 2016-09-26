#ifndef MEASUREMENT_OVERHEAD_C
#define MEASUREMENT_OVERHEAD_C
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sched.h>
//#include <preempt.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <fcntl.h>
#include <time.h>

int main(int agrc, char *argv[])
{
	unsigned cycles_high0; 
	unsigned cycles_low0; 
	unsigned cycles_high1; 
	unsigned cycles_low1; 
	uint64_t start,end;
	int sys;
	int fd;
	
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

	//preempt_disable();
	asm volatile ("cpuid\n\t"
		  "rdtsc\n\t"
		  "mov %%edx, %0\n\t"
		  "mov %%eax, %1\n\t"
		  : "=r" (cycles_high0), "=r" (cycles_low0)
		  :: "%rax", "%rbx", "%rcx", "%rdx");

	sys = syscall(SYS_dup,fd);

	
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
	printf("syscall dup overhead time = %llu clock cycles\n",(end-start));
	if(sys == -1)
		printf("FAILED\n");

	close(fd);

	//asm volatile ("cpuid\n\t"
	//	  "rdtsc\n\t"
	//	  "mov %%edx, %0\n\t"
	//	  "mov %%eax, %1\n\t"
	//	  : "=r" (cycles_high0), "=r" (cycles_low0)
	//	  :: "%rax", "%rbx", "%rcx", "%rdx");

	//sys = syscall(SYS_gettid);

	//
   //asm volatile ("rdtscp\n\t"
	//	  "mov %%edx, %0\n\t"
	//	  "mov %%eax, %1\n\t"
	//	  "cpuid\n\t"
	//	  : "=r" (cycles_high1), "=r" (cycles_low1)
	//	  :: "%rax", "%rbx", "%rcx", "%rdx");

	////preempt_enable();
	//start = ( ((uint64_t)cycles_high0 << 32) | cycles_low0 ); 
   //end = ( ((uint64_t)cycles_high1 << 32) | cycles_low1 ); 

	////printf("high0 = %d, high1 = %d, low0 = %d, low1 = %d\n",cycles_high0,cycles_high1,cycles_low0,cycles_low1);
	//printf("syscall gettid overhead time = %llu clock cycles\n",(end-start));
	//if(sys == -1)
	//	printf("FAILED\n");



}

#endif