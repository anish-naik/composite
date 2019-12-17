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
float m1[600][1000]; //60X100
float m2[1000][500]; //100X50
float final[600][500];
#define NTHDS NUM_CPU
short CACHE_ALIGNED counter= 0;
int CACHE_ALIGNED cpu[NUM_CPU] = {0};
int main ( void )
{
	int i, j, M, K, N;
	u64_t start_rt, end_rt;
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
	int m;
	float sum;
	rdtscll(start_rt);
	# pragma omp parallel private(m)
	{
	#pragma omp for 
    	for(m=0; m < 600; m++){
        	for(i = 0; i < 500; i++){
	            sum = 0;
	            for(j = 0; j < 1000; j++){
	                sum += m1[m][j] * m2[j][i];
	     	    }
	            final[m][i] = sum;
        	}
	}
	int c = ps_faa(&counter, 1);
        cpu[c] = cos_cpuid();
	}
	rdtscll(end_rt);
	/*int x, y;
	for(x=0; x<60; x++){
		for(y=0; y<50; y++){
			PRINTC("%f\n", final[x][y]);
		}
	}*/
	assert(counter == NUM_CPU);
        int bmp = 0;
        int d;
        for(d = 0; d < NUM_CPU; d++){
                bmp |= (1<<cpu[d]);
        }
        assert(bmp == (1<<NUM_CPU)-1);
        PRINTC("cycles=%d, rt=%lld", cos_hw_cycles_per_usec(BOOT_CAPTBL_SELF_INITHW_BASE), (end_rt - start_rt)/2100);
        return 0;
}
