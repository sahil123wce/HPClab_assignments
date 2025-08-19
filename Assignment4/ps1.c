#include <stdio.h>
#include <omp.h>

int main()
{
    int N;

    printf("Enter number of terms for Fibonacci series: ");
    scanf("%d", &N);

    if (N < 2)
    {
        printf("Number of terms must be at least 2.\n");
        return 0;
    }

    int fib[N];
    fib[0] = 0;
    fib[1] = 1;

    double start, end;

    // Serial
    start = omp_get_wtime();
    for (int i = 2; i < N; i++)
    {
        fib[i] = fib[i - 1] + fib[i - 2];
    }
    end = omp_get_wtime();

    printf("Serial Fibonacci: ");
    for (int i = 0; i < N; i++)
        printf("%d ", fib[i]);
    printf("\nSerial Time: %f seconds\n", end - start);

    // Parallel
    int fib_parallel[N];
    fib_parallel[0] = 0;
    fib_parallel[1] = 1;

    start = omp_get_wtime();
#pragma omp parallel
    {
        for (int i = 2; i < N; i++)
        {
#pragma omp critical // Ensuring only 1 thread updates at a time
            {
                fib_parallel[i] = fib_parallel[i - 1] + fib_parallel[i - 2];
            }
#pragma omp barrier // Ensuring all threads c updated val
        }
    }
    end = omp_get_wtime();

    printf("Parallel Fibonacci (with sync): ");
    for (int i = 0; i < N; i++)
        printf("%d ", fib_parallel[i]);
    printf("\nParallel Time: %f seconds\n", end - start);

    return 0;
}
