#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main()
{
    int n, m, scalar;
    printf("Enter number of rows: ");
    scanf("%d", &n);
    printf("Enter number of columns: ");
    scanf("%d", &m);
    printf("Enter scalar value: ");
    scanf("%d", &scalar);

    int A[n][m], B[n][m];

    printf("Enter matrix elements:\n");
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            scanf("%d", &A[i][j]);

    double start = omp_get_wtime();

#pragma omp parallel for collapse(2)
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            B[i][j] = scalar * A[i][j];

    double end = omp_get_wtime();

    printf("\nResultant Matrix:\n");
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
            printf("%d ", B[i][j]);
        printf("\n");
    }

    printf("\nTime taken: %f seconds\n", end - start);

    return 0;
}
