#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define VECTOR_SIZE 200
#define SCALAR 5

void vector_scalar_add_static(int chunk_size);
void vector_scalar_add_dynamic(int chunk_size);
void nowait_demo();

int main()
{
    int chunk_sizes[] = {1, 5, 10, 50, 100};
    int num_chunks = sizeof(chunk_sizes) / sizeof(chunk_sizes[0]);

    printf("\nStatic Scheduling\n");
    for (int i = 0; i < num_chunks; ++i)
    {
        vector_scalar_add_static(chunk_sizes[i]);
    }

    printf("\nDynamic Scheduling \n");
    for (int i = 0; i < num_chunks; ++i)
    {
        vector_scalar_add_dynamic(chunk_sizes[i]);
    }

    printf("\nNowait Demonstration\n");
    nowait_demo();

    return 0;
}

// Static scheduling with varying chunk sizes
void vector_scalar_add_static(int chunk_size)
{
    int vec[VECTOR_SIZE];
    for (int i = 0; i < VECTOR_SIZE; i++)
        vec[i] = i;

    double start_time = omp_get_wtime();

#pragma omp parallel for schedule(static, chunk_size)
    for (int i = 0; i < VECTOR_SIZE; i++)
    {
        vec[i] += SCALAR;
    }

    double end_time = omp_get_wtime();
    printf("Chunk size: %3d | Time: %.8f sec\n", chunk_size, end_time - start_time);
}

// Dynamic scheduling with varying chunk sizes
void vector_scalar_add_dynamic(int chunk_size)
{
    int vec[VECTOR_SIZE];
    for (int i = 0; i < VECTOR_SIZE; i++)
        vec[i] = i;

    double start_time = omp_get_wtime();

#pragma omp parallel for schedule(dynamic, chunk_size)
    for (int i = 0; i < VECTOR_SIZE; i++)
    {
        vec[i] += SCALAR;
    }

    double end_time = omp_get_wtime();
    printf("Chunk size: %3d | Time: %.8f sec\n", chunk_size, end_time - start_time);
}

// Demonstrating nowait clause
void nowait_demo()
{
    int A[VECTOR_SIZE], B[VECTOR_SIZE], C[VECTOR_SIZE];
    for (int i = 0; i < VECTOR_SIZE; i++)
    {
        A[i] = B[i] = i;
    }

    double start_time = omp_get_wtime();

#pragma omp parallel
    {
#pragma omp for nowait
        for (int i = 0; i < VECTOR_SIZE; i++)
        {
            A[i] += SCALAR;
        }

#pragma omp for nowait
        for (int i = 0; i < VECTOR_SIZE; i++)
        {
            B[i] *= SCALAR;
        }

#pragma omp for
        for (int i = 0; i < VECTOR_SIZE; i++)
        {
            C[i] = A[i] + B[i]; // no need to wait for previous loops due to nowait
        }
    }

    double end_time = omp_get_wtime();
    printf("Nowait applied | Time: %.8f sec\n", end_time - start_time);
}
