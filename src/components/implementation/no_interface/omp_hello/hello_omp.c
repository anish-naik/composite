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
void my_fn(int id);
//global variables
float m1[60][100]; //60X100
float m2[100][50]; //100X50
float final[60][50];

#define NTHDS NUM_CPU

int main ( void )
{
	int i, j, M, K, N, id;
	u64_t start, end;

	PRINTC("HELLO_OPENMP\n");
	PRINTC("Number of processors available is %d\n", omp_get_num_procs());
	PRINTC("Number of threads is %d\n", omp_get_max_threads());

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
	PRINTC("Going INSIDE the parallel region:\n");
	rdtscll(start);
	/*
  	INSIDE THE PARALLEL REGION, have each thread runs my_fn.
	*/
	# pragma omp parallel private ( id )
	{
		id = omp_get_thread_num();
		my_fn(id);
	}
	/*
  	Finish up by measuring the elapsed time.
	*/
	rdtscll(end);
	PRINTC ( "  Back OUTSIDE the parallel region.\n" );
	/*
	Terminate.
	*/
    // int x, y;
    // for(x = 0; x < M; x++){
    //     for(y = 0; y < K; y++){
    //         PRINTC("%d, %d, %f\n", x, y, final[x][y]);
    //     }
    // }
	PRINTC("diff is %lld\n", end-start);
	return 0;
}

void my_fn(int id){
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

