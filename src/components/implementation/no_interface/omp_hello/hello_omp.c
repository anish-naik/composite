#include <llprint.h>
#include <omp.h>

/******************************************************************************/
void my_fn(int id);
//global variables
float m1[60][100]; //60X100
float m2[100][50]; //100X50
float final[60][50];

int main ( void )
{
  int id;
  double wtime;

  PRINTC ( "\n" );
  PRINTC ( "HELLO_OPENMP\n" );
  PRINTC ( "  C/OpenMP version\n" );

  PRINTC ( "\n" );
  PRINTC ( "  Number of processors available = %d\n", omp_get_num_procs ( ) );
  PRINTC ( "  Number of threads =              %d\n", omp_get_max_threads ( ) );

  wtime = omp_get_wtime ( );

  PRINTC ( "\n" );
  PRINTC ( "  OUTSIDE the parallel region.\n" );
  PRINTC ( "\n" );

  id = omp_get_thread_num ( );
  PRINTC ( "  HELLO FROM PROCESS %d\n", id);
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
  PRINTC ( "\n" );
  PRINTC ( "  Going INSIDE the parallel region:\n" );
  PRINTC ( "\n" );
/*
  INSIDE THE PARALLEL REGION, have each thread say hello.
*/
# pragma omp parallel\
  private ( id )
  {
    my_fn(id);
  }
/*
  Finish up by measuring the elapsed time.
*/
  wtime = omp_get_wtime ( ) - wtime;

  PRINTC ( "\n" );
  PRINTC ( "  Back OUTSIDE the parallel region.\n" );
/*
  Terminate.
*/
  PRINTC ( "\n" );
  PRINTC ( "HELLO_OPENMP\n" );
  PRINTC ( "  Normal end of execution.\n" );

  PRINTC ( "\n" );
  PRINTC ( "  Elapsed wall clock time = %f\n", wtime );

  return 0;
}

void my_fn(int id){
        int low = (60 * id)/NTHDS;
        int high = (60 * (id+1))/NTHDS;
        int i, j;
        int sum;
        for(i = 0; i < 50; i++){
                if(low >= high){
                        break;
                }
                sum = 0;
                for(j = 0; j < 100; j++){
                        sum += m1[low][j] * m2[j][i];
                }
                final[low][i] = sum;
                low += 1;
        }
        return;
}

