#ifndef MEASUREMENT_OVERHEAD_C
#define MEASUREMENT_OVERHEAD_C
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sched.h>
//#include <prempt.h>
#include <sys/syscall.h>
#include <time.h>
#include <math.h>
#define ITER 20
#define CLOCK 1.7
int main(int agrc, char *argv[])
{
	unsigned cycles_high0; 
	unsigned cycles_low0; 
	unsigned cycles_high1; 
	unsigned cycles_low1; 
	uint64_t start,end;
	uint64_t sum,avg;
	volatile int i;
	uint64_t a_cycles[ITER],a_cycles_loop[ITER];
	uint64_t mean_cycles=0,var_cycles=0,mean_cycles_loop=0,var_cycles_loop=0;

	sum = 0;
	//Icache warmup
	asm volatile ("cpuid\n\t"
		  "rdtsc\n\t"
		  "mov %%edx, %0\n\t"
		  "mov %%eax, %1\n\t"
		  : "=r" (cycles_high0), "=r" (cycles_low0)
		  :: "%rax", "%rbx", "%rcx", "%rdx");


	for(int m=0;m< ITER;m++)
	{
	sum = 0;
	//preempt_disable();
	for(i = 0; i<1000; i++)
	{
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

		//printf("high0 = %d, high1 = %d, low0 = %d, low1 = %d\n",cycles_high0,cycles_high1,cycles_low0,cycles_low1);
		//printf("measurement overhead time = %llu clock cycles\n",(end-start));

		sum = sum + (end - start);
	}

	avg = (sum/1000);

	//printf("Average measurement overhead time = %lu clock cycles \n",avg);
	a_cycles[m] = avg;	
	mean_cycles += avg;


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

	//printf("high0 = %d, high1 = %d, low0 = %d, low1 = %d\n",cycles_high0,cycles_high1,cycles_low0,cycles_low1);
	//printf("cycles taken without loop = %llu clock cycles\n",cycles_without_loop);
	
	a=0;
	
	asm volatile ("cpuid\n\t"
		  "rdtsc\n\t"
		  "mov %%edx, %0\n\t"
		  "mov %%eax, %1\n\t"
		  : "=r" (cycles_high0), "=r" (cycles_low0)
		  :: "%rax", "%rbx", "%rcx", "%rdx");

	for(i=0;i<1000;i++);
		//a++;
	
	asm volatile ("rdtscp\n\t"
	  "mov %%edx, %0\n\t"
	  "mov %%eax, %1\n\t"
	  "cpuid\n\t"
	  : "=r" (cycles_high1), "=r" (cycles_low1)
	  :: "%rax", "%rbx", "%rcx", "%rdx");

	start = ( ((uint64_t)cycles_high0 << 32) | cycles_low0 ); 
	end = ( ((uint64_t)cycles_high1 << 32) | cycles_low1 ); 
	
	cycles_with_loop = end-start;
	
	//printf("high0 = %d, high1 = %d, low0 = %d, low1 = %d\n",cycles_high0,cycles_high1,cycles_low0,cycles_low1);
	//printf("cycles taken with loop = %lu clock cycles\n",(cycles_with_loop/1000));

	//printf("Thus the LOOP OVERHEAD = %llu clock cycles\n",(cycles_with_loop - cycles_without_loop));
	a_cycles_loop[m] = cycles_with_loop/1000; 
	mean_cycles_loop += a_cycles_loop[m];
	}

	mean_cycles      /= (ITER);
        mean_cycles_loop /= (ITER);

        for(i =0; i< ITER; i++)
        {
                 var_cycles      += ((a_cycles[i] - mean_cycles)*(a_cycles[i] - mean_cycles));
                 var_cycles_loop += ((a_cycles_loop[i] - mean_cycles_loop)*(a_cycles_loop[i] - mean_cycles_loop));
        }
        var_cycles /= (ITER);
        var_cycles_loop /= (ITER);

        printf("Mean time for measurement overhead %f ns, %lu cycles, SD %f ns %f cycles iterations %d\n",
	(double)mean_cycles/CLOCK,mean_cycles,sqrt(var_cycles)/CLOCK,sqrt(var_cycles),ITER);
        printf("Mean time for loop overhead %f ns, %lu cycles, SD %f ns %f cycles iterations %d\n",
	(double)mean_cycles_loop/CLOCK,mean_cycles_loop,sqrt(var_cycles_loop)/CLOCK,sqrt(var_cycles),ITER);

}

#endif
