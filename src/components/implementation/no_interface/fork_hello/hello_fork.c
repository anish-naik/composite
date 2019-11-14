#include <llprint.h>
#include <part.h>
#include <part_task.h>
#include <sl.h>
#include <sl_thd.h>
#include <sl_lock.h>
#include <crt_lock.h>
#include <cos_component.h>
#include <cos_alloc.h>


#define NTHDS 4
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
	M = 60;
	K = 50;
	N = 100;
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
	//struct part_data *d = part_data_alloc();
	part_fn_t fn = my_fn;
	void * data = NULL;
	part_task_init(pt, PART_TASK_T_WORKSHARE, p, n, my_fn, data, NULL);
	assert(pt->nthds == n); 	
	c->part_context = pt;
	//uint64_t start = rdtsc();
	part_list_append(pt);
	my_fn(data);
	part_task_end(pt);
	//uint64_t end = rdtsc();
	//PRINTC("start is %llu\n", start);
	//PRINTC("end is %llu\n", end);
	return 0;
}

void my_fn(void * data){
	int id = part_task_work_thd_num(pt, PART_CURR_THD);
	int low = (60 * id)/NTHDS;
	int high = (60 * (id+1))/NTHDS;	
	int i, j;
	int s[3];
	int sum;
	int row = low;
	for(i = 0; i < 50; i++){
		if(row >= high){
			break;
		}
		sum = 0;
		for(j = 0; j < 100; j++){
			sum += m1[row][j] * m2[j][i];
		}
		final[row][i] = sum;
		row += 1;
	}
	return;
}
