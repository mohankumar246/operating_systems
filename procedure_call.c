#ifndef PROCEDURE_CALL_C
#define PROCEDURE_CALL_C
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sched.h>
#include <preempt.h>
#include <sys/syscall.h>
#include <time.h>

//void call0()
//{
//	printf("I don't know what to do\n");
//}
// FIXME THIS CODE IS NOT WORKING PROPERLY 

void call2(int a1,int a2)
{
	float mean;
	printf("Lets take mean of this numbers\n");

	mean = (a1+a2)/2;

	printf("mean = %f\n",mean);

}
void call4(int a1,int a2,int a3,int a4)
{
	float mean;
	printf("Lets take mean of this numbers\n");

	mean = (float)(a1+a2+a3+a4)/4;

	printf("mean = %f\n",mean);
}

void call8(int a1,int a2,int a3,int a4,int a5,int a6,int a7,int a8)
{
	float mean;
	printf("Lets take mean of this numbers\n");

	mean = (float)(a1+a2+a3+a4+a5+a6+a7+a8)/8;

	printf("mean = %f\n",mean);
}

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

	call4(2,3,4,5);
	
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
	printf("call4 overhead = %llu clock cycles\n",(end-start));


	asm volatile ("cpuid\n\t"
		  "rdtsc\n\t"
		  "mov %%edx, %0\n\t"
		  "mov %%eax, %1\n\t"
		  : "=r" (cycles_high0), "=r" (cycles_low0)
		  :: "%rax", "%rbx", "%rcx", "%rdx");

	call4(1,2,3,4);
	
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
	printf("call8 overhead = %llu clock cycles\n",(end-start));



	

}

#endif
