
#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>
#include <sched.h>
#include <sys/syscall.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

#define ITER 20 
#define CLOCK 1700000000

int main(int agrc, char *argv[])
{

    register int child_id;
    int pipefd[2],pipefd_2[2];    

    register unsigned start_h,start_l,end_h,end_l,i;
    uint64_t a_proc_cycles[ITER],sum_proc_cycles=0;
    uint64_t a_cont_cycles[ITER],sum_cont_cycles=0;
    uint64_t a_cont_cycles_temp[ITER];
    uint64_t a_proc_cycles_temp[ITER];
    uint64_t mean_proc_cycles=0,var_proc_cycles=0;
    uint64_t mean_cont_cycles=0,var_cont_cycles=0;
    uint64_t mean_cont_cycles_temp=0;

    for(i=0;i<ITER;i++)
    {
	mean_cont_cycles_temp = 0;
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

        asm volatile ("CPUID\n\t"
                  "RDTSC\n\t"
                  "mov %%edx, %0\n\t"
                  "mov %%eax, %1\n\t": "=r" (start_h), "=r" (start_l)::
                  "%rax", "%rbx", "%rcx", "%rdx");

        child_id = fork();
        
	asm volatile("RDTSCP\n\t"
		     "mov %%edx, %0\n\t"
                     "mov %%eax, %1\n\t"
                     "CPUID\n\t": "=r" (end_h), "=r" (end_l)::
                     "%rax", "%rbx", "%rcx", "%rdx");

        uint64_t total_cycles = (((uint64_t)end_h << 32) | end_l )-(((uint64_t)start_h << 32) | start_l);

        //printf("Total cycles process creation %ld\n",total_cycles);
        a_proc_cycles[i] = total_cycles;
        mean_proc_cycles += total_cycles;

        if(child_id == -1)
        {
            perror("fork");
            exit(0);
        }

        //child
        if(child_id == 0)
        {
            unsigned cycles_low, cycles_high;
            uint64_t end;
            close(pipefd[1]);
            close(pipefd_2[0]);
            int k=0,a;
	    raise(SIGSTOP);
            while(k<ITER)
            {
                a =read(pipefd[0],&end,sizeof(end));
		asm volatile("RDTSCP\n\t"
                        "mov %%edx, %0\n\t"
                        "mov %%eax, %1\n\t"
                        "CPUID\n\t": "=r" (cycles_high), "=r" (cycles_low)::
                        "%rax", "%rbx", "%rcx", "%rdx");
                end = (((uint64_t)cycles_high << 32) | cycles_low);
                a   = write(pipefd_2[1],&end,sizeof(end));
                k++;
            }
        } //Parent
        else
        {
            unsigned cycles_low1, cycles_high1;
            uint64_t start=0,end;
            close(pipefd[0]);
            close(pipefd_2[1]);
            int k=0,a;
	    waitpid(child_id, NULL, WUNTRACED); // wait until the child is stopped
            kill(child_id, SIGCONT);

            while(k<ITER)
            {
                a = write(pipefd[1],&start,sizeof(start));

	        asm volatile ("CPUID\n\t"
                             "RDTSC\n\t"
                             "mov %%edx, %0\n\t"
                             "mov %%eax, %1\n\t": "=r" (cycles_high1), "=r" (cycles_low1)::
                             "%rax", "%rbx", "%rcx", "%rdx");

                a                = read(pipefd_2[0],&end,sizeof(end));
                start            = (((uint64_t)cycles_high1 << 32) | cycles_low1 );
		//if(k>0)
		{
			a_cont_cycles_temp[k] = end - start;	
			mean_cont_cycles_temp += a_cont_cycles_temp[k];
		}
		//printf("Cycles %ld\n",end-start);
                k++;
            }

	    //if(i> 0)
	    {			
		    a_cont_cycles[i] = mean_cont_cycles_temp / (ITER);//-1);	
		    mean_cont_cycles += a_cont_cycles[i];	
	    }
            close(pipefd[1]);
            close(pipefd_2[0]);

	    kill(child_id, SIGKILL);	
		
            //int returnStatus;    
            //waitpid(child_id, &returnStatus, 0);  // Parent process waits here for child to terminate.

            //if (returnStatus == 0)  // Verify child process terminated without error.  
            //{
            //   std::cout << "The child process terminated normally." << std::endl;    
            //}

            //if (returnStatus == 1)      
            //{
            //   std::cout << "The child process terminated with an error!." << std::endl;    
            //}
        }        
    }   
	
	if(child_id != 0)
	{
	    mean_proc_cycles /= (ITER);//-1);
	    mean_cont_cycles /= (ITER);//-1);

		printf("\nMean Cycles Proc %ld",mean_proc_cycles);
		printf("\nMean Cycles Cont %ld",mean_cont_cycles);
	    for(int i =0; i< ITER; i++)
	    {
		     var_proc_cycles += ((a_proc_cycles[i] - mean_proc_cycles)*(a_proc_cycles[i] - mean_proc_cycles));
		     var_cont_cycles += ((a_cont_cycles[i] - mean_cont_cycles)*(a_cont_cycles[i] - mean_cont_cycles));
	    }

	    var_proc_cycles /= (ITER);//-1);
	    var_cont_cycles /= (ITER);//-1);
	    
	    printf("\nMean time for process creation %f s SDeviation %f iteratios %d\n",(float)mean_proc_cycles/CLOCK,(float)sqrt(var_proc_cycles)/CLOCK,ITER);
	    printf("Mean time for context switch %f s SDeviation %f iteratios %d\n",(float)mean_cont_cycles/CLOCK,(float)sqrt(var_cont_cycles)/CLOCK,ITER);
	}
}
