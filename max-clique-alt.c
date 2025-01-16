#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

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

#define setUnion(A, B) \
    ((set){ (A).a[0] | (B).a[0], (A).a[1] | (B).a[1] })

#define setIntersection(A, B) \
    ((set){ (A).a[0] & (B).a[0], (A).a[1] & (B).a[1] })

#define setNegation(A) \
    ((set){ ~(A).a[0], ~(A).a[1] })

static inline int getBit(set A, int i) {
    if (i < ULL_SIZE) {
       return 1 & (A.a[0] >> i);
    } else {
       return 1 & (A.a[1] >> (i-ULL_SIZE));
    }
}

static inline set setBit(set A, int i) {
    if (i < ULL_SIZE) {
        return (set) {A.a[0] | (1ULL << i), A.a[1]};
    } else {
        return (set) {A.a[0], A.a[1] | (1ULL << (i-ULL_SIZE))};
    }
}

static inline set placeBit(set A, int b, int i) {
    if (i < ULL_SIZE) {
        return (set) {A.a[0] | (b << i), A.a[1]};
    } else {
        return (set) {A.a[0], A.a[1] | (b << (i-ULL_SIZE))};
    }
}

static inline set unsetBit(set A, int i) {
    if (i < ULL_SIZE) {
        return (set) {A.a[0] & ~(1ULL << i), A.a[1]};
    } else {
        return (set) {A.a[0], A.a[1] & ~(1ULL << (i-ULL_SIZE))};
    }
}

static inline int isEmpty(set A) {
    return A.a[0] == 0ULL && A.a[1] == 0ULL;
}

int popBit(set* A, int i) {
    int bit = getBit(*A, i);
    *A = unsetBit(*A, i);
    return bit;
}

// won't be called if isEmpty(A) is true
static inline int popFirstBit(set* A) {
    if (A->a[0] != 0ULL) {
        // least-significant set bit in lower word
        int i = __builtin_ctzll(A->a[0]);   
        A->a[0] &= ~(1ULL << i);
        return i;
    } else if (A->a[1] != 0ULL) {
        int i = __builtin_ctzll(A->a[1]);
        A->a[1] &= ~(1ULL << i);
        return (i + 64);   
    } else {
        // no bits set
        return -1;         
    }
}

static inline int popcount(set A) {
    return __builtin_popcountll(A.a[0]) + __builtin_popcountll(A.a[1]);
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

void toBitGraph(int graph[N_MAX][N_MAX], set* graphB, int n) {
    for (int i=0; i < n; i++) {
        set temp = {0,0};
        for (int j=0; j < n; j++) {
            temp = placeBit(temp, graph[i][j], j);
        }
        graphB[i] = temp;
    }
}

void printSetB(set A) {
    for (int i=0; i < N_MAX; i++) {
        printf("%d ", getBit(A, i));
    }
    printf("\n");
}

void printGraph(set graph[N_MAX], int n) {
    for (int i=0; i < n; i++) {
        printSetB(graph[i]);
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
int findPivot(set graph[N_MAX], int n, set P) {

    set pcopy = P;
    int max = 0;
    int ans = 0;

    while (!isEmpty(pcopy)) {
        int i = popFirstBit(&pcopy);
        int temp = popcount(graph[i]);
        if (temp > max) { max=temp; ans=i; }
    }

    return ans;
}

set getNeighbors(set graph[N_MAX], int p) {
    return graph[p];
}

set ans;
   
void help (set graph[N_MAX], int n, set P, set R, set X) {
    if (isEmpty(P) && isEmpty(X)) {
        // printSet(R);
        ans = compareSets(ans, R);
        return;
    }    

    int pivot = findPivot(graph, n, setUnion(P, X));
    set np = getNeighbors(graph, pivot);

    set pcopy = setIntersection(P, setNegation(np));

    while (!isEmpty(pcopy)) {
        int v = popFirstBit(&pcopy);

        set ni = getNeighbors(graph, v);

        help(graph, n, setIntersection(P, ni), setBit(R, v), setIntersection(X, ni));

        P = unsetBit(P, v);
        X = setBit(X, v);
    }
}

void findMaxClique(int graph[N_MAX][N_MAX], int n) {

    set P = {0, 0};
    for (int i=0; i < N; i++) P = setBit(P, i);
    set X = {0, 0};
    set R = {0, 0};

    toNotDirectedGraph(graph, n);
    set* graphB = (set*) malloc (N_MAX*sizeof(set));
    toBitGraph(graph, graphB, n);

    help (graphB, n, P, R, X);

    printf("Clique Members: ");
    printSet(ans);
    printf("Size: %d\n", popcount(ans));

    free(graphB);
}