#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <time.h>

#include "set.h"

void generateCombinations(uchar* graph, int n, int *clique, int k, int start, int currentSize, int *maxSize, int *maxClique);

// assume graph has no self loops

uchar* createGraph(int n) {
    uchar* graph = calloc(n*n, sizeof(uchar));

    for (int i=0; i < n; i++) {
        for (int j=0; j < i; j++) {
            // create edge with 0.5 prob
            graph[i*n + j] = (rand() > RAND_MAX/2);
        }
        // don't allow for loops
        for (int j=i+1; j < n; j++) {
            graph[i*n + j] = (rand() > RAND_MAX/2);
        }
    }

    return graph;
}

void toNotDirectedGraph(uchar* graph, int n) {
    for (int i=0; i < n; i++) {
        for (int j=i+1; j < n; j++) {
            uchar t = graph[i*n + j] || graph[j*n +i];
            graph[i*n + j] = t;
            graph[j*n + i] = t;
        }
    }
}

void printGraph(uchar* graph, int n) {
    for (int i=0; i < n; i++) {
        for (int j=0; j < n; j++) {
            printf("%d ", graph[i*n + j]);
        }
        printf("\n");
    }
    printf("\n");
}

void printSet(set A) {
    for (int i=0; i < N_MAX; i++) {
        if (getBit(A, i)) printf("%d ", i);
    }
    printf("\n");
}

// returns the node with max neighbors
int findPivot(uchar* graph, int n) {
    int max = 0;
    int ans = 0;
    for (int i=0; i < n; i++) {
        int temp = 0;
        for (int j=0; j < n; j++) {
            temp += graph[j*n + i];
        }
        if (temp > max) { max=temp; ans=i; }
    }

    return ans;
}

set getNeighbors(uchar* graph, int n, int p) {
    set ans = {0, 0};
    for (int i=0; i < n; i++) {
        if (graph[i*n+p]) ans = setBit(ans, i);
    }
    return ans;
}

set ans = {0,0};
    
void findMaxClique(uchar* graph, int n, set P, set R, set X) {
    if (isEmpty(X) && isEmpty(P)) { ans = compareSets(ans, R); return; }

    for (int i=0; i < n; i++) {
        if (!getBit(P, i)) continue;
        set ni = getNeighbors(graph, n, i);
        findMaxClique(graph, n, setIntersection(P, ni), setBit(R, i), setIntersection(X, ni));
        P = unsetBit(P, i);
        X = setBit(X, i);
    }
}

void benchmark(uchar* graph, algo f) {
    set P = {0, 0};
    for (int i=0; i < N; i++) P = setBit(P, i);
    set X = {0, 0};
    set R = {0, 0};

    double sum = 0;

    for (int i=0; i < I; i++) {
        clock_t start = clock();
        f(graph, N, P, X, R);
        sum += (double) (clock() - start) / (CLOCKS_PER_SEC);
    }
    
    printf("time taken: %lf\n", (double) sum / I);
    printSet(ans);
}

void benchmark2(uchar* graph) {

    int *clique = (int *)malloc(N * sizeof(int));
    int *maxClique = (int *)malloc(N * sizeof(int));
    int maxSize = 0;
    double sum = 0;

    for (int i=0; i < N; i++) { clique[i] = 0; maxClique[i] = 0; }

    for (int i=0; i < I; i++) {
        clock_t start = clock();

        for (int k = 1; k <= N; k++) {
            generateCombinations(graph, N, clique, k, 0, 0, &maxSize, maxClique);
        }

        sum += (double) (clock() - start) / (CLOCKS_PER_SEC);
    }
    
    printf("time taken: %lf\n", (double) sum / I);

    printf("Clique Members: ");
    for (int i=0; i < maxSize; i++) printf("%d ", maxClique[i]);
    printf("Size: %d\n", maxSize);

}

int main () {
    srand(time(NULL));
    uchar* graph = createGraph(N);
    toNotDirectedGraph(graph, N);
    printGraph(graph, N);
    printf("benchmarking good: ");
    benchmark(graph, findMaxClique);    
    printf("benchmarking bad: ");
    benchmark2(graph);    
}