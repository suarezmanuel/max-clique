#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <time.h>

#define uchar unsigned char
#define ull unsigned long long int
#define N_MAX 128
#define ULL_SIZE 64
#define N 20
#define I 1000

typedef struct set {
    // a[0] is lower, a[1] is higher
    ull a[2];
} set;

typedef void (*algo)(uchar*, int, set, set, set);

set setUnion(set A, set B) {
    return (set){A.a[0] | B.a[0], A.a[1] | B.a[1]};
}

set setIntersection(set A, set B) {
    return (set){A.a[0] & B.a[0], A.a[1] & B.a[1]};
}

uchar getBit(set A, int i) {
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

uchar isEmpty(set A) {
    return A.a[0] == 0ULL && A.a[1] == 0ULL;
}

uchar popBit(set* A, int i) {
    uchar bit = getBit(*A, i);
    *A = unsetBit(*A, i);
    return bit;
}

uchar popcount(set A) {
    uchar count = 0;
    for (int i=0; !isEmpty(A) && i < N_MAX; i++) { 
        count += popBit(&A, i);
    }
    return count;
}

// won't be called if isEmpty(A) is true
uchar popFirstBit(set* A) {
    for (int i=0; i < N_MAX; i++) {
        if (popBit(A, i)) return i;
    }
    // shouldn't happen
    return -1;
}

// is called if they have the same popcount
set compareSets (set A, set B) {

    uchar countA = popcount(A);
    uchar countB = popcount(B);
    if (countA > countB) {
        return A;
    } else if (countA < countB){
        return B;
    }

    uchar a, b;
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
        graph[i*n + i] = 0;
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
    // printf("set size: %d\n", popcount(A));
    // for (int i=N_MAX-1; i >= 0; i--) {
    //     printf("%d", getBit(A, i));
    //     if (i == ULL_SIZE) { printf (" "); }
    // }
    // printf("\n");
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

set ans;
    
void help (uchar* graph, int n, set P, set R, set X) {
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

void findMaxClique(uchar* graph, int n) {
    set P = {0, 0};
    for (int i=0; i < N; i++) P = setBit(P, i);
    set X = {0, 0};
    set R = {0, 0};

    toNotDirectedGraph(graph, n);

    help (graph, n, P, R, X);

    printSet(ans);
    printf("Size: %d\n", popcount(ans));
}

void benchmark(uchar* graph, algo f) {
    set P = {0, 0};
    for (int i=0; i < N; i++) P = setBit(P, i);
    set X = {0, 0};
    set R = {0, 0};

    double sum = 0;

    for (int i=0; i < I; i++) {
        ans = (set){0,0};
        clock_t start = clock();
        f(graph, N, P, X, R);
        sum += (double) (clock() - start) / (CLOCKS_PER_SEC);
    }
    
    printf("time taken: %lf\n", (double) sum / I);
}

// void test () {

//     int *clique    = (int *)malloc(N * sizeof(int));
//     int *maxClique = (int *)malloc(N * sizeof(int));

//     set P = {0, 0};
//     for (int i=0; i < N; i++) P = setBit(P, i);
//     set X = {0, 0};
//     set R = {0, 0};

//     int f = 1;

//     for (int i=0; i < I; i++) {
        
//         uchar* graph = createGraph(N);
//         toNotDirectedGraph(graph, N);

//         for (int i=0; i < N; i++) { clique[i] = 0; maxClique[i] = 0; }
//         int maxSize = 0;
//         ans = (set){0,0};

//         findMaxClique(graph, N, P, R, X);

//         for (int k = 1; k <= N; k++) {
//             generateCombinations(graph, N, clique, k, 0, 0, &maxSize, maxClique);
//         }

//         if (maxSize != popcount(ans)) f = 0;

//         for (int i=0; i < maxSize; i++) {
//             if (!getBit(ans, maxClique[i])) f = 0;
//         }

//         free (graph);
//     }

//     printf ("the functions are: %s\n", (f ? "equal!" : "different :("));
// }


// int main () {
    // srand(time(NULL));
    // test();
    // uchar* graph = createGraph(N);
    // toNotDirectedGraph(graph, N);
    // printGraph(graph, N);
    // benchmark(graph, findMa\xClique);    
    // printf("benchmarking good: ");
    // printSet(ans);
    // printf("benchmarking bad: ");
    // benchmark2(graph);    
// }