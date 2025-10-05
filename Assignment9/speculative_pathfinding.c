#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <omp.h>
#include <math.h>
#include <time.h>

#define V 100
#define REPEAT 1000

// to find the vertex with minm distance value
int min_distance(int dist[], int sptSet[])
{
    int min = INT_MAX, min_index;
    for (int v = 0; v < V; v++)
    {
        if (sptSet[v] == 0 && dist[v] <= min)
        {
            min = dist[v];
            min_index = v;
        }
    }
    return min_index;
}

// Sequential Dijkstra's algorithm
void dijkstra(int graph[V][V], int src, int dist[V])
{
    int sptSet[V];

    for (int i = 0; i < V; i++)
    {
        dist[i] = INT_MAX;
        sptSet[i] = 0;
    }
    dist[src] = 0;

    for (int count = 0; count < V - 1; count++)
    {
        int u = min_distance(dist, sptSet);
        sptSet[u] = 1;

        for (int v = 0; v < V; v++)
        {
            if (!sptSet[v] && graph[u][v] && dist[u] != INT_MAX && dist[u] + graph[u][v] < dist[v])
            {
                dist[v] = dist[u] + graph[u][v];
            }
        }
    }
}

// Heuristic function for A*
int heuristic(int node, int goal)
{
    int x1 = node % 10, y1 = node / 10;
    int x2 = goal % 10, y2 = goal / 10;
    return (int)sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)) * 2;
}

// A* algorithm implementation
void a_star(int graph[V][V], int src, int goal, int dist[V])
{
    int openSet[V] = {0};
    int closedSet[V] = {0};
    int gScore[V];
    int fScore[V];

    for (int i = 0; i < V; i++)
    {
        gScore[i] = INT_MAX;
        fScore[i] = INT_MAX;
        dist[i] = INT_MAX;
    }

    gScore[src] = 0;
    fScore[src] = heuristic(src, goal);
    openSet[src] = 1;
    dist[src] = 0;

    for (int count = 0; count < V - 1; count++)
    {
        // Find node with lowest fScore
        int current = -1;
        int min_f = INT_MAX;
        for (int i = 0; i < V; i++)
        {
            if (openSet[i] && fScore[i] < min_f)
            {
                min_f = fScore[i];
                current = i;
            }
        }

        if (current == -1 || current == goal)
            break;

        openSet[current] = 0;
        closedSet[current] = 1;

        for (int neighbor = 0; neighbor < V; neighbor++)
        {
            if (graph[current][neighbor] > 0 && !closedSet[neighbor])
            {
                int tentative_gScore = gScore[current] + graph[current][neighbor];

                if (!openSet[neighbor])
                {
                    openSet[neighbor] = 1;
                }

                if (tentative_gScore < gScore[neighbor])
                {
                    gScore[neighbor] = tentative_gScore;
                    fScore[neighbor] = gScore[neighbor] + heuristic(neighbor, goal);
                    dist[neighbor] = gScore[neighbor];
                }
            }
        }
    }
}

// Generate a random graph
void generate_random_graph(int graph[V][V])
{
    // Initialize with zeros
    for (int i = 0; i < V; i++)
    {
        for (int j = 0; j < V; j++)
        {
            graph[i][j] = 0;
        }
    }

    // Create a connected graph with random wts
    for (int i = 0; i < V; i++)
    {
        for (int j = i + 1; j < V; j++)
        {
            // 30% chance of having an edge between nodes
            if (rand() % 100 < 30)
            {
                int weight = (rand() % 50) + 1;
                graph[i][j] = weight;
                graph[j][i] = weight;
            }
        }
    }

    // Ensure the graph is connected
    for (int i = 0; i < V - 1; i++)
    {
        if (graph[i][i + 1] == 0)
        {
            int weight = (rand() % 50) + 1;
            graph[i][i + 1] = weight;
            graph[i + 1][i] = weight;
        }
    }
}

// to print partial solution
void print_solution_summary(int dist[V], int source, int target, char *algorithm)
{
    printf("%s: Distance from %d to %d = ", algorithm, source, target);
    if (dist[target] == INT_MAX)
    {
        printf("INF\n");
    }
    else
    {
        printf("%d\n", dist[target]);
    }
}

int main()
{
    int graph[V][V];
    int source = 0;
    int target = V - 1;

    srand(time(NULL));

    generate_random_graph(graph);

    int dist_dijkstra[V];
    int dist_astar[V];

    double start_dijkstra, end_dijkstra, start_astar, end_astar;
    double time_dijkstra = 0, time_astar = 0;
    double total_time_dijkstra = 0, total_time_astar = 0;
    int dijkstra_wins = 0, astar_wins = 0;

    printf("Speculative Pathfinding (Dijkstra vs A*)\n");
    printf("Graph Size: %d vertices, Repeating %d times for better timing\n", V, REPEAT);
    printf("Source: %d, Target: %d\n\n", source, target);

    // Run multiple times for better timing measurements
    for (int run = 0; run < REPEAT; run++)
    {
        int committed_algorithm = -1;
        int committed_dist[V];

// Speculative Decomposition: Run both algorithms in parallel
#pragma omp parallel sections
        {
#pragma omp section
            {
                start_dijkstra = omp_get_wtime();
                for (int i = 0; i < 10; i++)
                { // Inner loop to increase computation time
                    dijkstra(graph, source, dist_dijkstra);
                }
                end_dijkstra = omp_get_wtime();
                time_dijkstra = end_dijkstra - start_dijkstra;

#pragma omp critical
                {
                    total_time_dijkstra += time_dijkstra;
                    if (committed_algorithm == -1)
                    {
                        committed_algorithm = 0;
                        dijkstra_wins++;
                        for (int i = 0; i < V; i++)
                            committed_dist[i] = dist_dijkstra[i];
                    }
                }
            }

#pragma omp section
            {
                start_astar = omp_get_wtime();
                for (int i = 0; i < 10; i++)
                { // Inner loop to increase computation time
                    a_star(graph, source, target, dist_astar);
                }
                end_astar = omp_get_wtime();
                time_astar = end_astar - start_astar;

#pragma omp critical
                {
                    total_time_astar += time_astar;
                    if (committed_algorithm == -1)
                    {
                        committed_algorithm = 1;
                        astar_wins++;
                        for (int i = 0; i < V; i++)
                            committed_dist[i] = dist_astar[i];
                    }
                }
            }
        }
    }

    // Calculate avgs
    double avg_time_dijkstra = total_time_dijkstra / REPEAT;
    double avg_time_astar = total_time_astar / REPEAT;

    // Run once sequentially for results
    dijkstra(graph, source, dist_dijkstra);
    a_star(graph, source, target, dist_astar);

    printf("\nFinal Results (Single Run)\n");
    print_solution_summary(dist_dijkstra, source, target, "Dijkstra");
    print_solution_summary(dist_astar, source, target, "A*");

    printf("\nSpeculative Performance Analysis (Averaged over %d runs)\n", REPEAT);
    printf("Average Dijkstra Time: %.6f seconds\n", avg_time_dijkstra);
    printf("Average A* Time:       %.6f seconds\n", avg_time_astar);
    printf("Dijkstra wins: %d times, A* wins: %d times\n", dijkstra_wins, astar_wins);

    if (avg_time_dijkstra < avg_time_astar)
    {
        printf("Average Wasted Computation: A* wasted %.6f seconds\n",
               avg_time_astar - avg_time_dijkstra);
        printf("Speculative Benefit: Using faster algorithm (Dijkstra) saved %.6f seconds on average\n",
               avg_time_astar - avg_time_dijkstra);
    }
    else if (avg_time_astar < avg_time_dijkstra)
    {
        printf("Average Wasted Computation: Dijkstra wasted %.6f seconds\n",
               avg_time_dijkstra - avg_time_astar);
        printf("Speculative Benefit: Using faster algorithm (A*) saved %.6f seconds on average\n",
               avg_time_dijkstra - avg_time_astar);
    }
    else
    {
        printf("Average Wasted Computation: Both algorithms performed similarly\n");
    }

    return 0;
}