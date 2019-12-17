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
//global variables
struct part_task * pt = &main_task;
short CACHE_ALIGNED counter= 0;
int CACHE_ALIGNED cpu[NUM_CPU] = {0};
int main (void){
	u64_t start_init, end_init, start_rt, end_rt;
	struct sl_thd *c = sl_thd_curr();
	struct part_task *p = (struct part_task *)c->part_context;
	assert(p == NULL);
	pt->state = PART_TASK_S_ALLOCATED;
	part_fn_t fn = my_fn;
	void * data = NULL;
	part_task_init(pt, PART_TASK_T_WORKSHARE, p, NTHDS, my_fn, NULL, NULL);
	c->part_context = pt;
	rdtscll(start_rt);
	part_list_append(pt);
	my_fn(data);
	part_task_end(pt);
	rdtscll(end_rt);
	assert(counter == NUM_CPU);
        int bmp = 0;
        int d;
        for(d = 0; d < NUM_CPU; d++){
                bmp |= (1<<cpu[d]);
        }
        assert(bmp == (1<<NUM_CPU)-1);
	PRINTC("cycles=%d, rt=%lld", cos_hw_cycles_per_usec(BOOT_CAPTBL_SELF_INITHW_BASE), end_rt - start_rt);
	return 0;
}

void my_fn(void * data){
        int c = ps_faa(&counter, 1);
        cpu[c] = cos_cpuid();
	return;
}
