#ifndef PROCEDURE_CALL_C
#define PROCEDURE_CALL_C
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <sched.h>
//#include <preempt.h>
#include <sys/syscall.h>
#include <time.h>

//void call0()
//{
//	printf("I don't know what to do\n");
//}
// FIXME THIS CODE IS NOT WORKING PROPERLY 
unsigned cycles_high0; 
unsigned cycles_low0; 
unsigned cycles_high1; 
unsigned cycles_low1; 
uint64_t start,end;
uint64_t sum,avg;
uint64_t call_overhead,ret_overhead;


void call2(int a1,int a2)
{
	float mean;
	printf("Lets take mean of this numbers\n");

	mean = (a1+a2)/2;

	printf("mean = %f\n",mean);

}
void call_func(int a1,int a2,int a3,int a4,
					int a5,int a6,int a7,int a8,
					int a9,int a10,int a11,int a12,
					int a13,int a14,int a15,int a16,
					int a17,int a18,int a19,int a20,
					int a21,int a22,int a23,int a24,
					int a25,int a26,int a27,int a28,
					int a29,int a30,int a31,int a32)
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

	a1 = a1+ a5 + a9  + a13 + a17 + a21 + a25 + a29;
	a2 = a2+ a7 + a10 + a14 + a18 + a22 + a26 + a30;
	a3 = a3+ a6 + a11 + a15 + a19 + a23 + a27 + a31;
	a4 = a4+ a8 + a12 + a16 + a20 + a24 + a28 + a32;
	mean = (float)(a1+a2+a3+a4)/4;
	printf("mean = %f\n",mean);


	asm volatile ("cpuid\n\t"
		  "rdtsc\n\t"
		  "mov %%edx, %0\n\t"
		  "mov %%eax, %1\n\t"
		  : "=r" (cycles_high0), "=r" (cycles_low0)
		  :: "%rax", "%rbx", "%rcx", "%rdx");
}

uint64_t call8(int a1,int a2,int a3,int a4,int a5,int a6,int a7,int a8)
{
	float mean;
	//printf("Lets take mean of this numbers\n");
	asm volatile ("cpuid\n\t"
		  "rdtsc\n\t"
		  "mov %%edx, %0\n\t"
		  "mov %%eax, %1\n\t"
		  : "=r" (cycles_high0), "=r" (cycles_low0)
		  :: "%rax", "%rbx", "%rcx", "%rdx");

	mean = (float)(a1+a2+a3+a4+a5+a6+a7+a8)/8;
	printf("mean = %f\n",mean);

   asm volatile ("rdtscp\n\t"
	    "mov %%edx, %0\n\t"
	    "mov %%eax, %1\n\t"
	    "cpuid\n\t"
	    : "=r" (cycles_high1), "=r" (cycles_low1)
	    :: "%rax", "%rbx", "%rcx", "%rdx");
	start = ( ((uint64_t)cycles_high0 << 32) | cycles_low0 ); 
   end = ( ((uint64_t)cycles_high1 << 32) | cycles_low1 ); 
	return (end - start);
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

	  call_func(2,3,4,5,6,7,8,9,1,2,3,4,5,6,7,8,1,2,3,4,21,22,23,24,2,3,4,5,1,2,3,32);
		
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


	//sum = 0;
	//for(i =0;i<20; i++)
	//{
	//	asm volatile ("cpuid\n\t"
	//		  "rdtsc\n\t"
	//		  "mov %%edx, %0\n\t"
	//		  "mov %%eax, %1\n\t"
	//		  : "=r" (cycles_high0), "=r" (cycles_low0)
	//		  :: "%rax", "%rbx", "%rcx", "%rdx");

	//	body_time = call8(1,2,3,4,5,6,7,8);
	//	
   //	 asm volatile ("rdtscp\n\t"
	//		  "mov %%edx, %0\n\t"
	//		  "mov %%eax, %1\n\t"
	//		  "cpuid\n\t"
	//		  : "=r" (cycles_high1), "=r" (cycles_low1)
	//		  :: "%rax", "%rbx", "%rcx", "%rdx");

	//	//preempt_enable();
	//	start = ( ((uint64_t)cycles_high0 << 32) | cycles_low0 ); 
   //	end = ( ((uint64_t)cycles_high1 << 32) | cycles_low1 ); 

	//	//printf("high0 = %d, high1 = %d, low0 = %d, low1 = %d\n",cycles_high0,cycles_high1,cycles_low0,cycles_low1);
	//	//printf("call8 overhead = %llu clock cycles\n",(end-start));
	//	sum = sum + (end - start) - body_time;

	//}
	//avg = (sum/20);

	//printf("Average measurement overhead time for call8 = %llu clock cycles \n",avg);


}

#endif
