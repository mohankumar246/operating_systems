
#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <sched.h>
#include <sys/syscall.h>
#include <time.h>

int main(int agrc, char *argv[])
{

	int child_id;
	int pipefd[2],pipefd_2[2];	


	if(pipe(pipefd) == -1)
	{
		perror("pipe");
		exit(0);
	}

	if(pipe(pipefd_2) == -1)
	{
		perror("pipe");
		exit(0);
	}

	child_id = fork();
	
	if(child_id == -1)
	{
		perror("fork");
		exit(0);
	}

	//child
	if(child_id == 0)
	{
		unsigned cycles_low, cycles_high;
		uint64_t start;
		close(pipefd[1]);
		close(pipefd_2[0]);
		int i=0,j,a;
		while(i<10)
		{
			asm volatile ("CPUID\n\t"
				      "RDTSC\n\t"
				      "mov %%edx, %0\n\t"
				      "mov %%eax, %1\n\t": "=r" (cycles_high), "=r" (cycles_low)::
				      "%rax", "%rbx", "%rcx", "%rdx");
			
			start = ( ((uint64_t)cycles_high << 32) | cycles_low );
			printf("child %d %ld\n",j,start);
			a =read(pipefd[0],&j,sizeof(int));
			a =write(pipefd_2[1],&i,sizeof(int));
			i++;
		}
	} //Parent
	else
	{
		unsigned cycles_low1, cycles_high1;
		uint64_t end;
		close(pipefd[0]);
		close(pipefd_2[1]);
		int i=0,a;
		while(i<10)
		{
			a = write(pipefd[1],&i,sizeof(int));

			asm volatile("RDTSCP\n\t"
					"mov %%edx, %0\n\t"
					"mov %%eax, %1\n\t"
					"CPUID\n\t": "=r" (cycles_high1), "=r" (cycles_low1)::
					"%rax", "%rbx", "%rcx", "%rdx");
			end = ( ((uint64_t)cycles_high1 << 32) | cycles_low1 );
			printf("parent %d %ld\n",i,end);

			a= read(pipefd_2[0],&i,sizeof(int));
			i++;
		}
	}		

}
