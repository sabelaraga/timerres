#ifndef __UTIL_RDTSC__
#define __UTIL_RDTSC__

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
	uint32_t l;
	uint32_t h;
} rdtsc_timeval_t;

typedef struct{
	rdtsc_timeval_t t1;
	rdtsc_timeval_t t2;
} mytimer_t;

uint64_t inline getCycles (rdtsc_timeval_t t){
	return ( ((uint64_t)t.h<<32) | t.l);
}

#define __cpuid(level, a, b, c, d)              \
 __asm__ __volatile__("mov %%rbx, %%rdi;"			\
	  "cpuid\n\t"                           \
	  "mov %%rbx, %%rsi;"			\
	  "mov %%rdi, %%rbx;"			\
          : "=a" (a), "=S" (b), "=c" (c), "=d" (d)     \
          : "0" (level)			\
	  :"rdi");


#define RDTSC_GET_TIMESTAMP(t1)  __asm__ __volatile__ ("rdtsc" : "=a" (t1.l), "=d" (t1.h));
#define RDTSCP_GET_TIMESTAMP(t1)  __asm__ __volatile__ ("rdtscp" : "=a" (t1.l), "=d" (t1.h));

//from the Intel paper. They segfault after an undefined number of iterations because it is not taking care of all registers.
//Anyway, the variance is similar to the other version
//#define RDTSC_GET_TIMESTAMP(t1)  __asm__ __volatile__ (	"cpuid\n\t"\
							"rdtsc\n\t"\
							"mov %%edx, %0\n\t"\
							"mov %%eax, %1\n\t"\
							 : "=r" (t1.h), "=r" (t1.l)::"%rax","%rbx","%rcx","%rdx");
//#define RDTSCP_GET_TIMESTAMP(t1)  __asm__ __volatile__ ("rdtscp\n\t"\
							"mov %%edx, %0\n\t"\
							"mov %%eax, %1\n\t"\
							"cpuid\n\t"\
							: "=r" (t1.h), "=r" (t1.l)::"rax","rbx","rcx","rdx");





#define SIZE_OF_STAT 100000
#define BOUND_OF_LOOP 1000


void inline EmptyTimes (uint64_t ** times){
	uint64_t a,b,c,d;
	mytimer_t timer;
	int i,j;	
	uint64_t start, end;

	//warmup instruction cache
	__cpuid(0,a,b,c,d);
	RDTSC_GET_TIMESTAMP (timer.t1);
	RDTSCP_GET_TIMESTAMP(timer.t2);
	__cpuid(0,a,b,c,d);

	__cpuid(0,a,b,c,d);
	RDTSC_GET_TIMESTAMP (timer.t1);
	RDTSCP_GET_TIMESTAMP(timer.t2);
	__cpuid(0,a,b,c,d);

	for(j=0; j<BOUND_OF_LOOP; j++){
		for(i=0; i<SIZE_OF_STAT; i++){
			__cpuid(0,a,b,c,d);
			RDTSC_GET_TIMESTAMP (timer.t1);
			//Insert the function to measure here
			RDTSCP_GET_TIMESTAMP(timer.t2);
			__cpuid(0,a,b,c,d);
			
			start = getCycles(timer.t1);
			end   = getCycles(timer.t2);
			
			if((end-start)<0){
				fprintf(stderr,"ERROR IN TAKING THE TIME:\n \tloop(%d) \tstat(%d) \tstart= %llu, \tend=%llu\n", j,i,start,end);
				times[j][i]=0;
			}
			else{
				times[j][i]=end-start;
			}
		}
	}
	return;
}

void inline measured_loop(unsigned int n, volatile int *var){
	int k;
	for(k=0; k<n; k++){
		(*var)=1;
	}
}


void inline WorkTimes (uint64_t ** times){
	uint64_t a,b,c,d;
	mytimer_t timer;
	int i,j;	
	volatile int variable = 0; 
	uint64_t start, end;

	//warmup instruction cache
	__cpuid(0,a,b,c,d);
	RDTSC_GET_TIMESTAMP (timer.t1);
	RDTSCP_GET_TIMESTAMP(timer.t2);
	__cpuid(0,a,b,c,d);

	__cpuid(0,a,b,c,d);
	RDTSC_GET_TIMESTAMP (timer.t1);
	RDTSCP_GET_TIMESTAMP(timer.t2);
	__cpuid(0,a,b,c,d);

	for(j=0; j<BOUND_OF_LOOP; j++){
		for(i=0; i<SIZE_OF_STAT; i++){
			variable = 0;
			__cpuid(0,a,b,c,d);
			RDTSC_GET_TIMESTAMP (timer.t1);
			measured_loop(j, &variable);
			RDTSCP_GET_TIMESTAMP(timer.t2);
			__cpuid(0,a,b,c,d);
			
			start = getCycles(timer.t1);
			end   = getCycles(timer.t2);
			
			if((end-start)<0){
				fprintf(stderr,"ERROR IN TAKING THE TIME:\n \tloop(%d) \tstat(%d) \tstart= %llu, \tend=%llu, \tvariable=%u\n", j,i,start,end,variable);
				times[j][i]=0;
			}
			else{
				times[j][i]=end-start;
			}
		}
	}
	return;
}


uint64_t var_calc(uint64_t *inputs, int size){
	int i;
	uint64_t acc =0, previous = 0, temp_var =0;
	for(i=0; i<size; i++){
		if(acc<previous){
			fprintf(stderr,"OVERFLOW ERROR IN var_calc\n");
			return -1;	
		}
		previous=acc;
		acc += inputs[i];
	}
	acc *= acc;
	
	if(acc<previous){
		fprintf(stderr,"OVERFLOW ERROR IN var_calc\n");
		return -1;	
	}
	previous = 0;
	for(i=0; i<size; i++){
		if(temp_var<previous){
			fprintf(stderr,"OVERFLOW ERROR IN var_calc\n");
			return -1;	
		}
		previous = temp_var;
		temp_var += (inputs[i]*inputs[i]);
	}	
	temp_var =temp_var*size;
	if(temp_var<previous){
		fprintf(stderr,"OVERFLOW ERROR IN var_calc\n");
		return -1;	
	}
	temp_var=(temp_var-acc)/(((uint64_t)(size))*((uint64_t)(size)));
	return (temp_var);
}



#endif
