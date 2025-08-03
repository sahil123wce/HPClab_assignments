#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

void initialize_matrix(int **mat, int n)
{
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            mat[i][j] = rand() % 100;
}

void matrix_addition(int **a, int **b, int **result, int n)
{
#pragma omp parallel for collapse(2)
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            result[i][j] = a[i][j] + b[i][j];
}

int **allocate_matrix(int n)
{
    int **mat = (int **)malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++)
        mat[i] = (int *)malloc(n * sizeof(int));
    return mat;
}
int main()
{
    int sizes[] = {250, 500, 750, 1000, 2000};
    int threads[] = {1, 2, 4, 8};

    printf("MatrixSize\tThreads\tTime(seconds)\n");
    for (int s = 0; s < 5; s++)
    {
        int n = sizes[s];
        int **a = allocate_matrix(n);
        int **b = allocate_matrix(n);
        int **res = allocate_matrix(n);

        initialize_matrix(a, n);
        initialize_matrix(b, n);

        for (int t = 0; t < 4; t++)
        {
            omp_set_num_threads(threads[t]);

            double start = omp_get_wtime();
            matrix_addition(a, b, res, n);
            double end = omp_get_wtime();

            printf("%d\t\t%d\t%.6f\n", n, threads[t], end - start);
        }

        for (int i = 0; i < n; i++)
        {
            free(a[i]);
            free(b[i]);
            free(res[i]);
        }
        free(a);
        free(b);
        free(res);
    }
    return 0;
}
