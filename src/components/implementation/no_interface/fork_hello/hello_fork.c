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
float m1[60][100]; //60X100
float m2[100][50]; //100X50
float final[60][50];
int main (void){
	PRINTC("HELLO FORK!\n");
	int i, j, M, K, N;
	u64_t start, end;
	M = 60;
	N = 100;
	K = 50;
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
	int n = NTHDS > PART_MAX_PAR_THDS ? PART_MAX_PAR_THDS : NTHDS;
	assert(p == NULL);
	pt->state = PART_TASK_S_ALLOCATED;
	part_fn_t fn = my_fn;
	void * data = NULL;
	rdtscll(start);
	part_task_init(pt, PART_TASK_T_WORKSHARE, p, n, my_fn, NULL, NULL);
	assert(pt->nthds == n); 	
	c->part_context = pt;
	part_list_append(pt);
	my_fn(data);
	part_task_end(pt);
	rdtscll(end);
	// int x, y;
	// for(x = 0; x < M; x++){
 //        for(y = 0; y < K; y++){
 //            PRINTC("%d, %d, %f\n", x, y, final[x][y]);
 //    	}
 //   	}
	PRINTC("diff is %lld\n", end-start);\
	PRINTC("HELLO WE FINISH!\n");
	return 0;
}

void my_fn(void * data){
	int id = part_task_work_thd_num(pt, PART_CURR_THD);
	int low = (60 * id)/NTHDS;
	int high = (60 * (id+1))/NTHDS;	
	int i, j;
	float sum;
	while(low < high){
		for(i = 0; i < 50; i++){
			sum = 0;
			for(j = 0; j < 100; j++){
				sum += m1[low][j] * m2[j][i];
			}
			final[low][i] = sum;
		}		
		low += 1;
	}
	return;
}
