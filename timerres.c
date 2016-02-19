#include "util.h"



void benchmark (int work){
	uint64_t ** mytimes;
	int i=0,j=0,spurious=0,k=0;
	uint64_t *variances;
	uint64_t *min_values;
	uint64_t max_dev =0, min_time=0, max_time=0, prev_min=0, tot_var=0, max_dev_all=0, var_of_vars=0, var_of_mins=0; 
	
	mytimes =(uint64_t**) malloc(BOUND_OF_LOOP*sizeof(uint64_t*));
	if(!mytimes){
		fprintf(stderr,"Unable to allocate memory for mytimes \n");
		return;
	}
	
	for(j=0; j<BOUND_OF_LOOP; j++){
		mytimes[j] = (uint64_t*)malloc(SIZE_OF_STAT*sizeof(uint64_t));
		if(!mytimes[j]){
			fprintf(stderr,"Unable to allocate memory for mytimes[%d] \n",j);
			return;
		}
	}
	
	variances = malloc(BOUND_OF_LOOP*sizeof(uint64_t));
	if(!variances){
		fprintf(stderr,"Unable to allocate memory for variances \n");
		return;
	}
	min_values = malloc(BOUND_OF_LOOP*sizeof(uint64_t));
	if(!min_values){
		fprintf(stderr,"Unable to allocate memory for min_values \n");
		return;
	}

	if(!work) EmptyTimes(mytimes);
	else	  WorkTimes(mytimes);

	for(j=0; j<BOUND_OF_LOOP; j++){
		max_dev=0;
		min_time=0;
		max_time=0;

		for(i=0;i<SIZE_OF_STAT;i++){
		//	if(((min_time==0)||(min_time>mytimes[j][i]))&&(mytimes[j][i]!=0)) min_time=mytimes[j][i];
		if(((min_time==0)||(min_time>mytimes[j][i]))) min_time=mytimes[j][i];

			if(max_time<mytimes[j][i]) max_time = mytimes[j][i];
		}
		max_dev = max_time-min_time;
		min_values[j] = min_time;

		if((prev_min!=0)&&(prev_min>min_time)) spurious++;
		if(max_dev > max_dev_all) max_dev_all = max_dev;

		variances[j] = var_calc(mytimes[j],SIZE_OF_STAT);
		tot_var += variances[j];

		fprintf(stdout,"\nloop_size %d >>>> variance(cycles): %llu; max_deviation: %llu; min time: %llu", j,variances[j],max_dev,min_time);

		prev_min = min_time;
	}

	var_of_vars = var_calc(variances,BOUND_OF_LOOP);
	var_of_mins = var_calc(min_values,BOUND_OF_LOOP);


	if(work) fprintf(stdout, "\n total number of spurious min values = %d", spurious);
	fprintf(stdout, "\n total variance = %llu", (tot_var/BOUND_OF_LOOP));
	fprintf(stdout, "\n absloute max deviation = %llu", max_dev_all);
	fprintf(stdout, "\n variance of variances = %llu", var_of_vars);
	fprintf(stdout, "\n variance of minimum values = %llu", var_of_mins);
	fprintf(stdout, "\n");

	for(j=0; j<BOUND_OF_LOOP; j++) free(mytimes[j]);
	free(mytimes);
	free(variances);
	free(min_values);
	return;
}
	



int main(int argc, char** argv){
	fprintf(stdout,"STARTING EMPTY BENCHMARK\n");	
	benchmark(0);
	fprintf(stdout,"EMPTY BENCHMARK FINALIZED\n");
	fprintf(stdout,"STARTING WORK BENCHMARK\n");	
	benchmark(1);
	fprintf(stdout,"WORK BENCHMARK FINALIZED\n");
}
