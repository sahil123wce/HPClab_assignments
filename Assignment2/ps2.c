#include <stdio.h>
#include <omp.h>

int main()
{
    long long num_steps;
    double pi, step, x, sum = 0.0;
    int i;

    printf("Enter number of steps: ");
    scanf("%lld", &num_steps);

    printf("Enter number of threads: ");
    int num_threads;
    scanf("%d", &num_threads);
    omp_set_num_threads(num_threads);

    step = 1.0 / (double)num_steps;

    double start = omp_get_wtime();

#pragma omp parallel for private(x) reduction(+ : sum)
    for (i = 0; i < num_steps; i++)
    {
        x = (i + 0.5) * step;
        sum += 4.0 / (1.0 + x * x);
    }

    pi = step * sum;

    double end = omp_get_wtime();

    printf("Estimated value of Pi = %.15f\n", pi);
    printf("Time taken = %f seconds\n", end - start);
    return 0;
}
