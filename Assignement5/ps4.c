#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main()
{
    int n;
    printf("Enter size of array: ");
    scanf("%d", &n);

    int A[n], prefix[n];

    printf("Enter array elements:\n");
    for (int i = 0; i < n; i++)
        scanf("%d", &A[i]);

    double start = omp_get_wtime();

    int num_threads;
    int *sum_block;

#pragma omp parallel
    {
        int id = omp_get_thread_num();
        int nth = omp_get_num_threads();

#pragma omp single
        {
            num_threads = nth;
            sum_block = (int *)malloc(nth * sizeof(int));
        }

        int chunk_size = (n + nth - 1) / nth;
        int start_idx = id * chunk_size;
        int end_idx = (start_idx + chunk_size < n) ? start_idx + chunk_size : n;

        if (start_idx < n)
        {
            prefix[start_idx] = A[start_idx];
            for (int i = start_idx + 1; i < end_idx; i++)
                prefix[i] = prefix[i - 1] + A[i];
            sum_block[id] = prefix[end_idx - 1];
        }
    }

    for (int i = 1; i < num_threads; i++)
        sum_block[i] += sum_block[i - 1];

#pragma omp parallel
    {
        int id = omp_get_thread_num();
        int nth = omp_get_num_threads();
        int chunk_size = (n + nth - 1) / nth;
        int start_idx = id * chunk_size;
        int end_idx = (start_idx + chunk_size < n) ? start_idx + chunk_size : n;

        if (id > 0 && start_idx < n)
        {
            int add_val = sum_block[id - 1];
            for (int i = start_idx; i < end_idx; i++)
                prefix[i] += add_val;
        }
    }

    double end = omp_get_wtime();

    printf("\nPrefix Sum Array:\n");
    for (int i = 0; i < n; i++)
        printf("%d ", prefix[i]);
    printf("\n");

    printf("\nTime taken: %f seconds\n", end - start);

    free(sum_block);
    return 0;
}
