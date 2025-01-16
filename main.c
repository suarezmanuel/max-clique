#include <stdio.h>
#include <time.h>

#define MAX_VERTICES 100

void findMaxClique(int graph[MAX_VERTICES][MAX_VERTICES], int n);

int main() {
    int n;
    printf("Enter the number of vertices (max %d): ", MAX_VERTICES);
    scanf("%d", &n);

    int graph[MAX_VERTICES][MAX_VERTICES];
    printf("Enter the adjacency matrix:\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            scanf("%d", &graph[i][j]);
        }
    }

    clock_t begin = clock();

    findMaxClique(graph, n);

    clock_t end = clock();

    double time_spent = (double)(end - begin) / (double)CLOCKS_PER_SEC;
    
    printf("Time spent: %.10f\n", time_spent/I);

    return 0;
}
