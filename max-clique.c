#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_VERTICES 100

void printSolution(int vertices[], int size) {
    for (int i = 0; i < size; i++) {
        printf("%d ", vertices[i]);
    }
    printf("\n");
}

bool isClique(int graph[MAX_VERTICES][MAX_VERTICES], int clique[], int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (i == j) {
                continue;
            }
            if (!(graph[clique[i]][clique[j]] || graph[clique[j]][clique[i]])) {
                return false;
            }
        }
    }
    return true;
}

void generateCombinations(int graph[MAX_VERTICES][MAX_VERTICES], int n, int *clique, int k, int start, int currentSize, int *maxSize, int *maxClique) {
    if (currentSize == k) {
        if (isClique(graph, clique, k)) {
            if (k > *maxSize) {
                *maxSize = k;
                for (int i = 0; i < k; i++) {
                    maxClique[i] = clique[i];
                }
            }
        }
        return;
    }

    for (int i = start; i < n; i++) {
        clique[currentSize] = i;
        generateCombinations(graph, n, clique, k, i + 1, currentSize + 1, maxSize, maxClique);
    }
}

void findMaxClique(int graph[MAX_VERTICES][MAX_VERTICES], int n) {
    int *clique = (int *)malloc(n * sizeof(int));
    int *maxClique = (int *)malloc(n * sizeof(int));
    int maxSize = 0;

    for (int k = 1; k <= n; k++) {
        generateCombinations(graph, n, clique, k, 0, 0, &maxSize, maxClique);
    }
    printf("Clique Members: ");
    printSolution(maxClique, maxSize);
    printf("Size: %d\n", maxSize);

    free(clique);
    free(maxClique);
}
