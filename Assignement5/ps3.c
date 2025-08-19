#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main()
{
    int n, m;
    printf("Enter number of rows (n): ");
    scanf("%d", &n);
    printf("Enter number of columns (m): ");
    scanf("%d", &m);

    int A[n][m], X[m], Y[n];

    printf("Enter matrix elements (%d x %d):\n", n, m);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            scanf("%d", &A[i][j]);

    printf("Enter vector elements (%d):\n", m);
    for (int j = 0; j < m; j++)
        scanf("%d", &X[j]);

    double start = omp_get_wtime();

#pragma omp parallel for
    for (int i = 0; i < n; i++)
    {
        int sum = 0;
        for (int j = 0; j < m; j++)
            sum += A[i][j] * X[j];
        Y[i] = sum;
    }

    double end = omp_get_wtime();

    printf("\nResultant Vector:\n");
    for (int i = 0; i < n; i++)
        printf("%d ", Y[i]);
    printf("\n");

    printf("\nTime taken: %f seconds\n", end - start);

    return 0;
}
