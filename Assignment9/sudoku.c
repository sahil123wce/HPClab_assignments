#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <stdbool.h>

#define N 9
#define UNASSIGNED 0

// Function to print the Sudoku grid
void print_grid(int grid[N][N])
{
    for (int row = 0; row < N; row++)
    {
        for (int col = 0; col < N; col++)
        {
            printf("%2d", grid[row][col]);
        }
        printf("\n");
    }
}

// Checks if it's safe to place 'num' at grid[row][col]
bool is_safe(int grid[N][N], int row, int col, int num)
{
    // Check row
    for (int x = 0; x < N; x++)
    {
        if (grid[row][x] == num)
            return false;
    }
    // Check column
    for (int x = 0; x < N; x++)
    {
        if (grid[x][col] == num)
            return false;
    }
    // Check 3x3 box
    int startRow = row - row % 3;
    int startCol = col - col % 3;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (grid[i + startRow][j + startCol] == num)
                return false;
        }
    }
    return true;
}

// Sequential backtracking solver (for comparison)
bool solve_sudoku_sequential(int grid[N][N])
{
    int row, col;
    bool found_empty = false;

    // Find the first unassigned location
    for (row = 0; row < N; row++)
    {
        for (col = 0; col < N; col++)
        {
            if (grid[row][col] == UNASSIGNED)
            {
                found_empty = true;
                break;
            }
        }
        if (found_empty)
            break;
    }
    // If no unassigned location, puzzle is solved
    if (!found_empty)
        return true;

    // Consider digits 1 to 9
    for (int num = 1; num <= 9; num++)
    {
        if (is_safe(grid, row, col, num))
        {
            grid[row][col] = num; // Tentative assignment
            if (solve_sudoku_sequential(grid))
            {
                return true; // Success
            }
            grid[row][col] = UNASSIGNED; // Failure, backtrack
        }
    }
    return false; // Triggers backtracking
}

// Structure to pass data to parallel tasks
typedef struct
{
    int grid[N][N];
    int row;
    int col;
    int num;
    bool *solution_found; // Shared flag to indicate if solution is found
} task_data;

// Worker function for each parallel task
bool solve_parallel(task_data *task)
{
    // Create a local copy of the grid for this speculative task
    int local_grid[N][N];
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            local_grid[i][j] = task->grid[i][j];
        }
    }

    int row = task->row;
    int col = task->col;
    int num = task->num;

    if (is_safe(local_grid, row, col, num))
    {
        local_grid[row][col] = num;
        if (solve_sudoku_sequential(local_grid))
        {
// Check if another task already found a solution
#pragma omp critical
            {
                if (!*(task->solution_found))
                {
                    *(task->solution_found) = true;
                    // Copy the solved grid back to the original
                    for (int i = 0; i < N; i++)
                    {
                        for (int j = 0; j < N; j++)
                        {
                            task->grid[i][j] = local_grid[i][j];
                        }
                    }
                }
            }
            return true;
        }
    }
    return false;
}

// Parallel Sudoku solver using OpenMP tasks (Exploratory Decomposition)
bool solve_sudoku_parallel(int grid[N][N])
{
    int row, col;
    bool found_empty = false;
    int candidate_count = 0;
    int candidates[9];
    bool solution_found = false;

    // Find the first unassigned cell
    for (row = 0; row < N; row++)
    {
        for (col = 0; col < N; col++)
        {
            if (grid[row][col] == UNASSIGNED)
            {
                found_empty = true;
                break;
            }
        }
        if (found_empty)
            break;
    }
    if (!found_empty)
        return true; // Solved

    // Find all possible candidates for this cell
    for (int num = 1; num <= 9; num++)
    {
        if (is_safe(grid, row, col, num))
        {
            candidates[candidate_count++] = num;
        }
    }

// Exploratory Decomposition: Create a parallel task for each candidate
#pragma omp parallel shared(solution_found)
    {
#pragma omp single
        {
            for (int i = 0; i < candidate_count; i++)
            {
                int num = candidates[i];
#pragma omp task firstprivate(num) shared(solution_found, grid)
                {
                    if (!solution_found)
                    { // Early termination check
                        task_data task;
                        for (int i = 0; i < N; i++)
                        {
                            for (int j = 0; j < N; j++)
                            {
                                task.grid[i][j] = grid[i][j];
                            }
                        }
                        task.row = row;
                        task.col = col;
                        task.num = num;
                        task.solution_found = &solution_found;
                        solve_parallel(&task);
                    }
                }
            }
        }
    }

    return solution_found;
}

int main()
{
    // A Hard Sudoku puzzle for testing
    int grid[N][N] = {
        {5, 3, 0, 0, 7, 0, 0, 0, 0},
        {6, 0, 0, 1, 9, 5, 0, 0, 0},
        {0, 9, 8, 0, 0, 0, 0, 6, 0},
        {8, 0, 0, 0, 6, 0, 0, 0, 3},
        {4, 0, 0, 8, 0, 3, 0, 0, 1},
        {7, 0, 0, 0, 2, 0, 0, 0, 6},
        {0, 6, 0, 0, 0, 0, 2, 8, 0},
        {0, 0, 0, 4, 1, 9, 0, 0, 5},
        {0, 0, 0, 0, 8, 0, 0, 7, 9}};

    int grid_seq[N][N];
    // Copy grid for sequential solver
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            grid_seq[i][j] = grid[i][j];
        }
    }

    printf("Original Sudoku puzzle:\n");
    print_grid(grid);

    // Time sequential execution
    double start_seq = omp_get_wtime();
    bool solved_seq = solve_sudoku_sequential(grid_seq);
    double end_seq = omp_get_wtime();

    if (solved_seq)
    {
        printf("\nSequential Solution found in %.6f seconds:\n", end_seq - start_seq);
        print_grid(grid_seq);
    }
    else
    {
        printf("\nSequential: No solution exists.\n");
    }

    // Time parallel execution
    double start_par = omp_get_wtime();
    bool solved_par = solve_sudoku_parallel(grid);
    double end_par = omp_get_wtime();

    if (solved_par)
    {
        printf("\nParallel Solution found in %.6f seconds:\n", end_par - start_par);
        print_grid(grid);
    }
    else
    {
        printf("\nParallel: No solution exists.\n");
    }

    printf("\nPerformance Summary \n");
    printf("Sequential Time: %.6f seconds\n", end_seq - start_seq);
    printf("Parallel Time:   %.6f seconds\n", end_par - start_par);
    if ((end_par - start_par) > 0)
    {
        printf("Speedup:         %.2fx\n", (end_seq - start_seq) / (end_par - start_par));
    }
    else
    {
        printf("Speedup:         Very large (parallel time too small to measure)\n");
    }

    return 0;
}