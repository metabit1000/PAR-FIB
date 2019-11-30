#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <tareador.h>

// N and MIN must be powers of 2
long N;
long MIN_SORT_SIZE;
long MIN_MERGE_SIZE;

#define BLOCK_SIZE 128L // reduced for Tareador analysis

#define T int

void basicsort(long n, T data[n]);

void basicmerge(long n, T left[n], T right[n], T result[n*2], long start, long length);

void merge(long n, T left[n], T right[n], T result[n*2], long start, long length) {
        if (length < MIN_MERGE_SIZE*2L) {
                // Base case
                tareador_start_task("base_1");
                basicmerge(n, left, right, result, start, length);
                tareador_end_task("base_1");
        } else {
                // Recursive decomposition
                tareador_start_task("merge_3");
                merge(n, left, right, result, start, length/2);
 				tareador_end_task("merge_3");
				tareador_start_task("merge_4");
                merge(n, left, right, result, start + length/2, length/2);
				tareador_end_task("merge_4");
        }
}

void multisort(long n, T data[n], T tmp[n]) {
        if (n >= MIN_SORT_SIZE*4L) {
                // Recursive decomposition
                tareador_start_task("multisort_0");
                multisort(n/4L, &data[0], &tmp[0]);
				tareador_end_task("multisort_0");

				tareador_start_task("multisort_1");
                multisort(n/4L, &data[n/4L], &tmp[n/4L]);
				tareador_end_task("multisort_1");

				tareador_start_task("multisort_2");
                multisort(n/4L, &data[n/2L], &tmp[n/2L]);
				tareador_end_task("multisort_2");

				tareador_start_task("multisort_3");
                multisort(n/4L, &data[3L*n/4L], &tmp[3L*n/4L]);
				tareador_end_task("multisort_3");

				tareador_start_task("merge_0");
                merge(n/4L, &data[0], &data[n/4L], &tmp[0], 0, n/2L);
				tareador_end_task("merge_0");

				tareador_start_task("merge_1");
                merge(n/4L, &data[n/2L], &data[3L*n/4L], &tmp[n/2L], 0, n/2L);
				tareador_end_task("merge_1");

				tareador_start_task("merge_2");
                merge(n/2L, &tmp[0], &tmp[n/2L], &data[0], 0, n);
				tareador_end_task("merge_2");
	} else {
		// Base case
		tareador_start_task("base_2");
		basicsort(n, data);
		tareador_end_task("base_2");
	}
}

static void initialize(long length, T data[length]) {
   long i;
   for (i = 0; i < length; i++) {
      if (i==0) {
         data[i] = rand();
      } else {
         data[i] = ((data[i-1]+1) * i * 104723L) % N;
      }
   }
}

static void clear(long length, T data[length]) {
   long i;
   for (i = 0; i < length; i++) {
      data[i] = 0;
   }
}

void check_sorted(long n, T data[n]) 
{
   int unsorted=0;
   for (int i=1; i<n; i++)
      if (data[i-1] > data[i]) unsorted++;
   if (unsorted > 0)
      printf ("\nERROR: data is NOT properly sorted. There are %d unordered positions\n\n",unsorted);
   else {
//      printf ("data IS ordered; ");
   }
}

int main(int argc, char **argv) {

        /* Defaults for command line arguments */
        N = 32768 * BLOCK_SIZE;
        MIN_SORT_SIZE = 32 * BLOCK_SIZE;
        MIN_MERGE_SIZE = 32 * BLOCK_SIZE;;

        /* Process command-line arguments */
        for (int i=1; i<argc; i++) {
              if (strcmp(argv[i], "-n")==0) {
                              N = atol(argv[++i]) * BLOCK_SIZE;
              }
              else if (strcmp(argv[i], "-s")==0) {
                              MIN_SORT_SIZE = atol(argv[++i]) * BLOCK_SIZE;
              }
              else if (strcmp(argv[i], "-m")==0) {
                              MIN_MERGE_SIZE = atol(argv[++i]) * BLOCK_SIZE;
              }
              else {
                      fprintf(stderr, "Usage: %s [-n vector_size -s MIN_SORT_SIZE -m MIN_MERGE_SIZE]\n", argv[0]);
                      fprintf(stderr, "       -n to specify the size of the vector (in Kelements) to sort (default 32768)\n");
                      fprintf(stderr, "       -s to specify the size of the vector (in Kelements) that breaks recursion in the sort phase (default 32)\n");
                      fprintf(stderr, "       -m to specify the size of the vector (in Kelements) that breaks recursion in the merge phase (default 32)\n");
                      return EXIT_FAILURE;
              }
        }

        fprintf(stdout, "Arguments (Kelements): N=%ld, MIN_SORT_SIZE=%ld, MIN_MERGE_SIZE=%ld\n", N/BLOCK_SIZE, MIN_SORT_SIZE/BLOCK_SIZE, MIN_MERGE_SIZE/BLOCK_SIZE);

	T *data = malloc(N*sizeof(T));
	T *tmp = malloc(N*sizeof(T));
	
	initialize(N, data);
	clear(N, tmp);

	tareador_ON();
   	multisort(N, data, tmp);
	tareador_OFF();

   	check_sorted (N, data);

    	fprintf(stdout, "Multisort program finished\n");
	return 0;
}
