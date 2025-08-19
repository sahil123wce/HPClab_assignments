#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main()
{
    int n, threads;
    printf("Enter matrix size (n for n x n): ");
    scanf("%d", &n);
    printf("Enter number of threads: ");
    scanf("%d", &threads);

    int **A = (int **)malloc(n * sizeof(int *));
    int **B = (int **)malloc(n * sizeof(int *));
    int **C = (int **)malloc(n * sizeof(int *));

    for (int i = 0; i < n; i++)
    {
        A[i] = (int *)malloc(n * sizeof(int));
        B[i] = (int *)malloc(n * sizeof(int));
        C[i] = (int *)malloc(n * sizeof(int));
    }

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
        {
            A[i][j] = rand() % 10;
            B[i][j] = rand() % 10;
            C[i][j] = 0;
        }

    double start = omp_get_wtime();
    omp_set_num_threads(threads);

#pragma omp parallel for collapse(2)
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            for (int k = 0; k < n; k++)
                C[i][j] += A[i][k] * B[k][j];

    double end = omp_get_wtime();
    printf("Time taken: %f seconds\n", end - start);

    for (int i = 0; i < n; i++)
    {
        free(A[i]);
        free(B[i]);
        free(C[i]);
    }
    free(A);
    free(B);
    free(C);

    return 0;
}
