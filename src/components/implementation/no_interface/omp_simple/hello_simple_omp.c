#include <llprint.h>
#include <omp.h>
#include <sl.h>
#include <sl_thd.h>
#include <sl_lock.h>
#include <crt_lock.h>
#include <cos_component.h>
#include <cos_alloc.h>
#include <inttypes.h>
#include <stdint.h>

/******************************************************************************/
//global variables
#define NTHDS NUM_CPU
short CACHE_ALIGNED counter= 0;
int CACHE_ALIGNED cpu[NUM_CPU] = {0};
int main ( void )
{
	u64_t start_rt, end_rt;
	rdtscll(start_rt);
	# pragma omp parallel
	{
	int c = ps_faa(&counter, 1);
        cpu[c] = cos_cpuid();
	}
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
