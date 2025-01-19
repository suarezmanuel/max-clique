#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>
#include <math.h>
 
#define N_MAX 100
#define ULL_SIZE 64

typedef __uint128_t set;
__uint128_t _1 = 1;
__uint128_t _0 = 0;

typedef void (*algo)(int*, int, set, set, set);

#define setUnion(a, b)         ((a) | (b))
#define setIntersection(a, b)  ((a) & (b))
#define setNegation(a)         (~(a))
#define getBit(a, i)           ((int)(_1 & ((a) >> (i))))
#define setBit(a, i)           ((a) | (_1 << (i)))
#define placeBit(a, b, i)      ((a) | (((set)(b)) << (i)))
#define unsetBit(a, i)         ((a) & ~(_1 << (i)))
#define isEmpty(a)             ((a) == _0)

int popBit(set* A, int i) {
    int bit = getBit(*A, i);
    *A = unsetBit(*A, i);
    return bit;
}

// won't be called if isEmpty(A) is true
static inline int popFirstBit(set* a) {
    uint64_t lower = *a;
    uint64_t upper = *a >> 64;

    if (lower != 0ULL) {
        // least-significant set bit in lower word
        int i = __builtin_ctzll(lower);   
        *a &= ~(_1 << i);
        return i;
    } else if (upper != 0) {
        int i = __builtin_ctzll(upper);
        *a &= ~(_1 << (i+64));
        return i + 64;   
    } else {
        // no bits set
        return -1;       
    }
}

static inline int popcount(set a) {
    return __builtin_popcountll((uint64_t) a) + __builtin_popcountll((uint64_t) (a >> 64));
}

// is called if they have the same popcount
set compareSets (set a, set b) {

    int countA = popcount(a);
    int countB = popcount(b);

    if (countA != countB) {
        return (countA > countB) ? a : b;
    }

    int a_bit, b_bit;
    set tempA = a;
    set tempB = b;

    // same popcount
    for (int i=0; i < countA; i++) {
        a_bit = popFirstBit(&tempA);
        b_bit = popFirstBit(&tempB);
        if (a_bit != b_bit) break;
    }

    return (a_bit < b_bit) ? a : b; 
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
        set temp = 0;
        for (int j=0; j < n; j++) {
            temp = placeBit(temp, graph[i][j], j);
        }
        graphB[i] = temp;
    }
}

void printSetB(set a) {
    for (int i=0; i < N_MAX; i++) {
        printf("%d ", getBit(a, i));
    }
    printf("\n");
}

void printGraph(set graph[N_MAX], int n) {
    for (int i=0; i < n; i++) {
        printSetB(graph[i]);
    }
    printf("\n");
}

void printSet(set a) {
    for (int i=0; i < N_MAX; i++) {
        if (getBit(a, i)) printf("%d ", i);
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

    set P = _0;
    set X = _0;
    set R = _0;

    for (int i=0; i < n; i++) P = setBit(P, i);

    toNotDirectedGraph(graph, n);
    set* graphB = (set*) malloc (N_MAX*sizeof(set));
    toBitGraph(graph, graphB, n);

    help (graphB, n, P, R, X);

    printf("Clique Members: ");
    printSet(ans);
    printf("Size: %d\n", popcount(ans));

    free(graphB);
}
