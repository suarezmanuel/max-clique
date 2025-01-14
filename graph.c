#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "set.h"

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
        // graph[i*n+p] is either 1 or 0
        ans.a[i<ULL_SIZE] = ans.a[i<ULL_SIZE] | (graph[i*n+p] << i);
    }
    return ans;
}

set ans = {0,0};
    
void findMaxClique(uchar* graph, int n, set P, set R, set X) {
    if (isEmpty(X) && isEmpty(P)) { ans = compareSets(ans, R); return; }

    int p = findPivot(graph, n);
    set np = getNeighbors(graph, n, p);

    for (int i=0; i < n; i++) {
        // nodes that arent p's neighbors, no loops in graph
        if (getBit(np, i)) continue;
        set ni = getNeighbors(graph, n, i);
        findMaxClique(graph, n, setIntersection(P, ni), setBit(R, i), setIntersection(X, ni));
        P = unsetBit(P, i);
        X = setBit(X, i);
    }
}

int main () {
    srand(time(NULL));
    uchar* graph = createGraph(N);
    printGraph(graph, N);
    set P = {0, 20};
    set X = {0, 0};
    set R = {0, 0};
    printf("%d \n", findPivot(graph,N));
    printSet(getNeighbors(graph,N,0));
    printf("%d \n", popcount(getNeighbors(graph,N,0)));
    findMaxClique(graph, N, P, X, R);
    printSet(ans);
}