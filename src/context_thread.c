
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
#include <math.h>

#define ITER 20
#define CLOCK 1700

typedef struct pipe_args
{
  int pipe_pc[2];
  int pipe_cp[2];
}pipe_args; 

uint64_t a_cont_cycles[ITER];
uint64_t mean_cont_cycles=0;
int i=0;

void* parent_thread(void *param)
{
      pipe_args *pipe_pc_cp = (pipe_args *)param;    	
      unsigned cycles_low1, cycles_high1;
      uint64_t start=0,end=0;
      int k=0,a;
	    uint64_t mean_cont_cycles_temp=0;
	    uint64_t a_cont_cycles_temp[ITER];

      //close(pipe_pc_cp->pipe_pc[0]);
      //close(pipe_pc_cp->pipe_cp[1]);
      //printf("Parent\n");
      while(k<ITER)
      {
              //printf("writing p %d\n",k);
              a = write(pipe_pc_cp->pipe_pc[1],&start,sizeof(start));
              //printf("p %d\n",a);
              asm volatile ("CPUID\n\t"
                             "RDTSC\n\t"
                             "mov %%edx, %0\n\t"
                             "mov %%eax, %1\n\t": "=r" (cycles_high1), "=r" (cycles_low1)::
                             "%rax", "%rbx", "%rcx", "%rdx");
              //printf("parent %d %ld\n",i,end);
              //printf("p %d\n",k);

              a= read(pipe_pc_cp->pipe_cp[0],&end,sizeof(end));
              start = (((uint64_t)cycles_high1 << 32) | cycles_low1 );
              if(k>0)
              {
                a_cont_cycles_temp[k] = end - start;
                mean_cont_cycles_temp += a_cont_cycles_temp[k];
                //printf("Cycles %ld\n",end-start);
              }
              k++;
      }

      //close(pipe_pc_cp->pipe_pc[1]);
      //close(pipe_pc_cp->pipe_cp[0]);
      a_cont_cycles[i] = mean_cont_cycles_temp / (ITER-1);    
      mean_cont_cycles += a_cont_cycles[i];


}

void* child_thread(void *param)
{
    pipe_args *pipe_pc_cp = (pipe_args *)param;    	
    unsigned cycles_low, cycles_high;
    uint64_t end=0;
    int k=0,a;
    //close(pipe_pc_cp->pipe_pc[1]);
    //close(pipe_pc_cp->pipe_cp[0]);
    //printf("Child\n");
    while(k<ITER)
    {
      //        printf("reading c %d\n",k);
      a= read(pipe_pc_cp->pipe_pc[0],&end,sizeof(end));
      asm volatile("RDTSCP\n\t"
                    "mov %%edx, %0\n\t"
                    "mov %%eax, %1\n\t"
                    "CPUID\n\t": "=r" (cycles_high), "=r" (cycles_low)::
                    "%rax", "%rbx", "%rcx", "%rdx");

      //printf("c %d\n",k);
      end = (((uint64_t)cycles_high << 32) | cycles_low);
      a = write(pipe_pc_cp->pipe_cp[1],&end,sizeof(end));
      k++;
    }
  
    //close(pipe_pc_cp->pipe_pc[0]);
    //close(pipe_pc_cp->pipe_cp[1]);

}


void* empty_thread(void *param)
{

}
int main(int agrc, char *argv[])
{

  int child_id;
	pipe_args pipe_pc_cp;
	pthread_t thread0,thread1,thread_empty;
	unsigned start_h,start_l,end_h,end_l;
	uint64_t a_thrd_cycles[ITER];
	uint64_t a_thrd_cycles_temp[ITER];
	uint64_t mean_thrd_cycles=0,var_thrd_cycles=0;
	uint64_t var_cont_cycles=0;

	for(i=0;i<ITER;i++)
	{

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
	
    asm volatile ("CPUID\n\t"
                  "RDTSC\n\t"
                  "mov %%edx, %0\n\t"
                  "mov %%eax, %1\n\t": "=r" (start_h), "=r" (start_l)::
                  "%rax", "%rbx", "%rcx", "%rdx");

    pthread_create(&thread_empty, NULL,empty_thread,&child_id);

    asm volatile("RDTSCP\n\t"
                 "mov %%edx, %0\n\t"
                 "mov %%eax, %1\n\t"
                 "CPUID\n\t": "=r" (end_h), "=r" (end_l)::
                 "%rax", "%rbx", "%rcx", "%rdx");

    uint64_t total_cycles = (((uint64_t)end_h << 32) | end_l )-(((uint64_t)start_h << 32) | start_l);

    //printf("Total cycles s creation %ld\n",total_cycles);
	
    a_thrd_cycles[i] = total_cycles;
    mean_thrd_cycles += total_cycles;

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

	  //printf("Threads joined exiting\n");
  }

  {
      mean_thrd_cycles /= (ITER);//-1);
      mean_cont_cycles /= (ITER);//-1);

      printf("\nMean Cycles Thrd %ld",mean_thrd_cycles);
      printf("\nMean Cycles Cont %ld",mean_cont_cycles);

      for(i =0; i< ITER; i++)
      {
          var_thrd_cycles += ((a_thrd_cycles[i] - mean_thrd_cycles)*(a_thrd_cycles[i] - mean_thrd_cycles));
          var_cont_cycles += ((a_cont_cycles[i] - mean_cont_cycles)*(a_cont_cycles[i] - mean_cont_cycles));
      }

      var_thrd_cycles /= (ITER);//-1);
      var_cont_cycles /= (ITER);//-1);

      printf("\nMean time for thrd creation %f micro s SDeviation %f iteratios %d\n",(float)mean_thrd_cycles/CLOCK,(float)sqrt(var_thrd_cycles)/CLOCK,ITER);
      printf("Mean time for context switch %f micro s SDeviation %f iteratios %d\n",(float)mean_cont_cycles/CLOCK,(float)sqrt(var_cont_cycles)/CLOCK,ITER);
  }
	
	return 0;

}
     
