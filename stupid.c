#include <stdbool.h>
#include "set.h"

bool isClique(uchar* graph, int clique[], int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (i == j) {
                continue;
            }
            if (!(graph[clique[i] * N + clique[j]] || graph[clique[j] * N + clique[i]])) {
                return false;
            }
        }
    }
    return true;
}

void generateCombinations(uchar* graph, int n, int *clique, int k, int start, int currentSize, int *maxSize, int *maxClique) {
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
