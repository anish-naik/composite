#include <llprint.h>
#include <part.h>
#include <part_task.h>
#include <sl.h>
#include <sl_thd.h>
#include <sl_lock.h>
#include <crt_lock.h>
#include <cos_component.h>
#include <cos_alloc.h>
#include <inttypes.h>
#include <stdint.h>

#define NTHDS NUM_CPU
/******************************************************************************/
//function definitions
void my_fn(void * data);
void print_matrix(int R, int C);
//global variables
struct part_task * pt = &main_task;
float m1[600][1000]; //60X100
float m2[1000][500]; //100X50
float final[600][500];
short CACHE_ALIGNED counter= 0;
int CACHE_ALIGNED cpu[NUM_CPU] = {0};
int main (void){
	int i, j, M, K, N;
	u64_t start_init, end_init, start_rt, end_rt;
	M = 600;
	N = 1000;
	K = 500;
	for(i = 0; i < M; i++){
        for(j = 0; j < N; j++){
            m1[i][j] = ((float)rand())/((float)RAND_MAX);
        }
    }
    for(i = 0; i < N; i++){
        for(j = 0; j < K; j++){
            m2[i][j] = ((float)rand())/((float)RAND_MAX);
    	}
   	}
	struct sl_thd *c = sl_thd_curr();
	struct part_task *p = (struct part_task *)c->part_context;
	assert(p == NULL);
	pt->state = PART_TASK_S_ALLOCATED;
	part_fn_t fn = my_fn;
	void * data = NULL;
	rdtscll(start_init);
	part_task_init(pt, PART_TASK_T_WORKSHARE, p, NTHDS, my_fn, NULL, NULL); 	rdtscll(end_init);
	c->part_context = pt;
	rdtscll(start_rt);
	part_list_append(pt);
	my_fn(data);
	part_task_end(pt);
	rdtscll(end_rt);
	/*
        PRINTC("%d, %d, %f\n", 599, 499, final[599][499]); 
    	*/
	assert(counter == NUM_CPU);
	int bmp = 0;
	int d;
	for(d = 0; d < NUM_CPU; d++){
		bmp |= (1<<cpu[d]);
	}
	assert(bmp == (1<<NUM_CPU)-1);
	PRINTC("cycles=%d, init=%lld, rt=%lld", cos_hw_cycles_per_usec(BOOT_CAPTBL_SELF_INITHW_BASE), end_init - start_init, (end_rt - start_rt)/2100);
	return 0;
}

void my_fn(void * data){
	int id = part_task_work_thd_num(pt, PART_CURR_THD);
	int low = (600 * id)/NTHDS;
	int high = (600 * (id+1))/NTHDS;	
	int i, j;
	float sum;
	while(low < high){
		for(i = 0; i < 500; i++){
			sum = 0;
			for(j = 0; j < 1000; j++){
				sum += m1[low][j] * m2[j][i];
			}
			final[low][i] = sum;
		}		
		low += 1;
	}
	int c = ps_faa(&counter, 1);
	cpu[c] = cos_cpuid();
	return;
}
