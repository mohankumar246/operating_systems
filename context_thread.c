
#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <sched.h>
#include <sys/syscall.h>
#include <time.h>
#include <pthread.h>


typedef struct pipe_args
{
  int pipe_pc[2];
  int pipe_cp[2];
}pipe_args; 

void* parent_thread(void *param)
{
      pipe_args *pipe_pc_cp = (pipe_args *)param;    	
      unsigned cycles_low1, cycles_high1;
      uint64_t end;
      int i=0,a;
      close(pipe_pc_cp->pipe_pc[0]);
      close(pipe_pc_cp->pipe_cp[1]);
      while(i<10)
      {
              a = write(pipe_pc_cp->pipe_pc[1],&i,sizeof(int));

              asm volatile("RDTSCP\n\t"
                              "mov %%edx, %0\n\t"
                              "mov %%eax, %1\n\t"
                              "CPUID\n\t": "=r" (cycles_high1), "=r" (cycles_low1)::
                             "%rax", "%rbx", "%rcx", "%rdx");
              end = ( ((uint64_t)cycles_high1 << 32) | cycles_low1 );
              printf("parent %d %ld\n",i,end);

              a= read(pipe_pc_cp->pipe_cp[0],&i,sizeof(int));
              i++;
      }


}

void* child_thread(void *param)
{
       pipe_args *pipe_pc_cp = (pipe_args *)param;    	
       unsigned cycles_low, cycles_high;
       uint64_t start;
       int i=0,j,a;
       close(pipe_pc_cp->pipe_pc[1]);
       close(pipe_pc_cp->pipe_cp[0]);
       while(i<10)
       {
         asm volatile ("CPUID\n\t"
                       "RDTSC\n\t"
                       "mov %%edx, %0\n\t"
                       "mov %%eax, %1\n\t": "=r" (cycles_high), "=r" (cycles_low)::
                       "%rax", "%rbx", "%rcx", "%rdx");

         start = ( ((uint64_t)cycles_high << 32) | cycles_low );
         printf("child %d %ld\n",j,start);
         a= read(pipe_pc_cp->pipe_pc[0],&i,sizeof(int));
         a = write(pipe_pc_cp->pipe_cp[1],&i,sizeof(int));
         i++;
       }

}

int main(int agrc, char *argv[])
{

        int child_id;
	pipe_args pipe_pc_cp;
	pthread_t thread0,thread1;


        if(pipe(pipe_pc_cp.pipe_pc) == -1)
        {
                perror("pipe");
                exit(0);
        }

        if(pipe(pipe_pc_cp.pipe_cp) == -1)
        {
                perror("pipe");
                exit(0);
        }
	
	if(pthread_create(&thread0, NULL, parent_thread,&pipe_pc_cp))
	{
		fprintf(stderr, "Error creating thread\n");
		return 1;
	}

	if(pthread_create(&thread1, NULL, child_thread,&pipe_pc_cp))
	{
		fprintf(stderr, "Error creating thread\n");
		return 1;
	
	}


	if(pthread_join(thread0,NULL))
	{
		fprintf(stderr,"Error joining thread0\n");
	}

	if(pthread_join(thread1,NULL))
	{
		fprintf(stderr,"Error joining thread1\n");
	}


	printf("Threads joined exiting\n");
	
	return 0;

}
     
