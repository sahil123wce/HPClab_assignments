#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int compare_asc(const void *a, const void *b)
{
    return (*(int *)a - *(int *)b);
}

int compare_desc(const void *a, const void *b)
{
    return (*(int *)b - *(int *)a);
}

int main()
{
    int size;
    printf("Enter size of the vectors: ");
    scanf("%d", &size);

    int a[size], b[size];
    int min_scalar_product = 0;

    printf("Enter elements of vector A:\n");
    for (int i = 0; i < size; i++)
    {
        scanf("%d", &a[i]);
    }

    printf("Enter elements of vector B:\n");
    for (int i = 0; i < size; i++)
    {
        scanf("%d", &b[i]);
    }

    // Sorting
    qsort(a, size, sizeof(int), compare_asc);
    qsort(b, size, sizeof(int), compare_desc);

    double start_time = omp_get_wtime();

#pragma omp parallel for reduction(+ : min_scalar_product) schedule(static)
    for (int i = 0; i < size; i++)
    {
        min_scalar_product += a[i] * b[i];
    }

    double end_time = omp_get_wtime();

    printf("Minimum Scalar Product: %d\n", min_scalar_product);
    printf("Execution Time: %f seconds\n", end_time - start_time);

    return 0;
}
