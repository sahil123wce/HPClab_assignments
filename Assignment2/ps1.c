#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main()
{
    int n, scalar, i;
    double start, end;

    printf("Enter size of vector: ");
    scanf("%d", &n);

    printf("Enter scalar value: ");
    scanf("%d", &scalar);

    int *vector = (int *)malloc(n * sizeof(int));

    for (i = 0; i < n; i++)
    {
        vector[i] = i;
    }

    printf("Enter number of threads: ");
    int num_threads;
    scanf("%d", &num_threads);
    omp_set_num_threads(num_threads);

    start = omp_get_wtime();

#pragma omp parallel for
    for (i = 0; i < n; i++)
    {
        vector[i] += scalar;
    }

    end = omp_get_wtime();

    printf("Sample Output (first 10 values):\n");
    for (i = 0; i < (n < 10 ? n : 10); i++)
    {
        printf("%d ", vector[i]);
    }

    printf("\nTime taken: %f seconds\n", end - start);
    free(vector);
    return 0;
}
