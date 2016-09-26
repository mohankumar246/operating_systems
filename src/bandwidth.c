#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <math.h>
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
#define ITER 20
int main(int agrc, char *argv[])
{

	register char *source = malloc(SIZE);
	register char *dest   = malloc(SIZE);
	unsigned start_h,start_l,end_h,end_l;
	uint64_t time_c;
	float bw_memcp[ITER],bw_memst[ITER];
	float var_memcp=0,var_memst=0;
	float mean_memcp=0,mean_memst=0;
	
	for(int i=0;i< ITER;i++)
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

		if(i > 0)
		{
			bw_memst[i]  = ((float)50 * SPEED)/time_c;
			mean_memst    += bw_memst[i];
		}
//		printf("Time for memset %f, BW %f MB/s\n",(float)time_c/SPEED, ((float)50 * SPEED)/time_c);	 

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

 //		printf("Time for memcpy %f, BW %f MB/s\n",(float)time_c/SPEED, ((float)50 * SPEED)/time_c);	 

		if(i > 0)
		{
			bw_memcp[i]  = ((float)50 * SPEED)/time_c;
			mean_memcp    += bw_memcp[i];
		}

	}
	mean_memcp /= (ITER-1);
	mean_memst /= (ITER-1);
	
	for(int i =1; i< ITER; i++)
	{
		 var_memcp += ((bw_memcp[i] - mean_memcp)*(bw_memcp[i] - mean_memcp));
		 var_memst += ((bw_memst[i] - mean_memst)*(bw_memst[i] - mean_memst));
	}
	var_memcp /= (ITER-1);
	var_memst /= (ITER-1);
	
	printf("Mean BW for memset %f MB/s SDeviation %f iteratios %d\n",mean_memst,sqrt(var_memst),ITER-1);
	printf("Mean BW for memcpy %f MB/s SDeviation %f iteratios %d\n",mean_memcp,sqrt(var_memcp),ITER-1);
}
