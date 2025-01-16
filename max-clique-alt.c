#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <time.h>

#define ull unsigned long long int
#define N_MAX 100
#define ULL_SIZE 64
#define N 20
#define I 1000

typedef struct set {
    // a[0] is lower, a[1] is higher
    ull a[2];
} set;

typedef void (*algo)(int*, int, set, set, set);

set setUnion(set A, set B) {
    return (set){A.a[0] | B.a[0], A.a[1] | B.a[1]};
}

set setIntersection(set A, set B) {
    return (set){A.a[0] & B.a[0], A.a[1] & B.a[1]};
}

int getBit(set A, int i) {
    if (i < ULL_SIZE) {
        return 1ULL & (A.a[0] >> i);
    } else {
        return 1ULL & (A.a[1] >> (i-ULL_SIZE));
    }
}

set setBit(set A, int i) {
    if (i < ULL_SIZE) {
        A.a[0] |= (1ULL << i);
    } else {
        A.a[1] |= (1ULL << (i-ULL_SIZE));
    }
    return A;
}

set unsetBit(set A, int i) {
    if (i < ULL_SIZE) {
        A.a[0] &= ~(1ULL << i);
    } else {
        A.a[1] &= ~(1ULL << (i-ULL_SIZE));
    }
    return A;
}

int isEmpty(set A) {
    return A.a[0] == 0ULL && A.a[1] == 0ULL;
}

int popBit(set* A, int i) {
    int bit = getBit(*A, i);
    *A = unsetBit(*A, i);
    return bit;
}

int popcount(set A) {
    int count = 0;
    for (int i=0; !isEmpty(A) && i < N_MAX; i++) { 
        count += popBit(&A, i);
    }
    return count;
}

// won't be called if isEmpty(A) is true
int popFirstBit(set* A) {
    for (int i=0; i < N_MAX; i++) {
        if (popBit(A, i)) return i;
    }
    // shouldn't happen
    return -1;
}

// is called if they have the same popcount
set compareSets (set A, set B) {

    int countA = popcount(A);
    int countB = popcount(B);
    if (countA > countB) {
        return A;
    } else if (countA < countB){
        return B;
    }

    int a, b;
    set tempA = A;
    set tempB = B;

    // same popcount
    for (int i=0; i < countA; i++) {
        a = popFirstBit(&tempA);
        b = popFirstBit(&tempB);
        if (a != b) break;
    }

    if (a < b) {
        return A;
    } else {
        return B;
    }
}

void toNotDirectedGraph(int graph[N_MAX][N_MAX], int n) {
    for (int i=0; i < n; i++) {
        graph[i][i] = 0;
        for (int j=i+1; j < n; j++) {
            int t = graph[i][j] || graph[j][i];
            graph[i][j] = t;
            graph[j][i] = t;
        }
    }
}

void printGraph(int graph[N_MAX][N_MAX], int n) {
    for (int i=0; i < n; i++) {
        for (int j=0; j < n; j++) {
            printf("%d ", graph[i][j]);
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
int findPivot(int graph[N_MAX][N_MAX], int n) {
    int max = 0;
    int ans = 0;
    for (int i=0; i < n; i++) {
        int temp = 0;
        for (int j=0; j < n; j++) {
            temp += graph[j][i];
        }
        if (temp > max) { max=temp; ans=i; }
    }

    return ans;
}

set getNeighbors(int graph[N_MAX][N_MAX], int n, int p) {
    set ans = {0, 0};
    for (int i=0; i < n; i++) {
        if (graph[i][p]) ans = setBit(ans, i);
    }
    return ans;
}

set ans;
    
void help (int graph[N_MAX][N_MAX], int n, set P, set R, set X) {
    if (isEmpty(P) && isEmpty(X)) {
        // printSet(R);
        ans = compareSets(ans, R);
        return;
    }

    set pcopy = P;

    while (!isEmpty(pcopy)) {
        int v = popFirstBit(&pcopy);

        set ni = getNeighbors(graph, n, v);

        help(graph, n, setIntersection(P, ni), setBit(R, v), setIntersection(X, ni));

        P = unsetBit(P, v);
        X = setBit(X, v);
    }
}

void findMaxClique(int graph[N_MAX][N_MAX], int n) {

    printf("\n");
    printGraph(graph, n);

    set P = {0, 0};
    for (int i=0; i < N; i++) P = setBit(P, i);
    set X = {0, 0};
    set R = {0, 0};

    toNotDirectedGraph(graph, n);

    help (graph, n, P, R, X);

    printSet(ans);
    printf("Size: %d\n", popcount(ans));
}
