#include <stdio.h>
#include <omp.h>

int main()
{
    int num_threads;

    printf("Enter number of threads: ");
    scanf("%d", &num_threads);

    omp_set_num_threads(num_threads);

    printf("\nSequential:\n"); // seq
    for (int i = 0; i < num_threads; i++)
        printf("Hello World from iteration %d! (Sequential)\n", i);

    printf("\nParallel:\n"); // parallel
#pragma omp parallel
    {
        int tid = omp_get_thread_num();
        printf("Hello World from thread %d! (Parallel)\n", tid);
    }

    return 0;
}
