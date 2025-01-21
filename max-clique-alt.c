    #include <stdio.h>
    #include <stdlib.h>
    #include <inttypes.h>
    #include <time.h>
    #include <math.h>
    
    #define N_MAX 100
    #define ULL_SIZE 64

    typedef union __attribute__((aligned(16))) {
        __uint128_t set;
        struct {
            uint64_t low;
            uint64_t high;
        } parts;
    } set;

    __uint128_t _1 = 1;
    __uint128_t _0 = 0;

    typedef void (*algo)(int*, int, set, set, set);

    #define setUnion(a, b, res)         ((res)->set = ((a)->set) | ((b)->set))
    #define setIntersection(a, b, res)  ((res)->set = ((a)->set) & ((b)->set))
    #define setNegation(a, res)         ((res)->set = (~((a)->set)))
    #define getBit(a, i)                (_1 & (((a)->set) >> i))
    #define setBit(a, i, res)           ((res)->set = (((a)->set) | (_1 << i)))
    #define placeBit(a, b, i, res)      ((res)->set = (a)->set | ((__uint128_t)(b) << (i)))
    #define unsetBit(a, i, res)         ((res)->set = ((a)->set) & ~(_1 << i))
    #define isEmpty(a)                  (((a)->set) == _0)

    int popBit(set* A, int i) {
        int bit = getBit(A, i);
        unsetBit(A, i, A);
        return bit;
    }

    // won't be called if isEmpty(A) is true
    static inline int popFirstBit(set* a) {

        if (a->parts.low != 0ULL) {
            // least-significant set bit in lower word
            int i = __builtin_ctzll(a->parts.low);   
            a->set &= ~(_1 << i);
            return i;
        } else if (a->parts.high) {
            int i = __builtin_ctzll(a->parts.high);
            a->set &= ~(_1 << (i+64));
            return i + 64;   
        } else {
            // no bits set
            return -1;       
        }
    }

    static inline int popcount(set a) {
        return __builtin_popcountll(a.parts.low) + __builtin_popcountll(a.parts.high);
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
            set temp = {_0};
            for (int j=0; j < n; j++) {
                placeBit(&temp, graph[i][j], j, &temp);
            }
            graphB[i] = temp;
        }
    }

    void printSetB(set a) {
        for (int i=0; i < N_MAX; i++) {
            printf("%d ", getBit(&a, i));
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
            if (getBit(&a, i)) printf("%d ", i);
        }
        printf("\n");
    }

    // returns the node with max neighbors
    int findPivot(set graph[N_MAX], int n, set* P, int degrees[N_MAX]) {

        set pcopy = *P;
        int max = 0;
        int ans = 0;

        while (!isEmpty(&pcopy)) {
            int i = popFirstBit(&pcopy);
            int temp = degrees[i];
            if (temp > max) { max=temp; ans=i; }
        }

        return ans;
    }

    void getNeighbors(set graph[N_MAX], int p, set* res) {
        *res = graph[p];
    }

    set ans;
    
    void help (set graph[N_MAX], int n, set* P, set* R, set* X, int degrees [N_MAX]) {
        if (isEmpty(P) && isEmpty(X)) {
            // printSet(R);
            ans = compareSets(ans, *R);
            return;
        }    

        set U, np, pcopy, nnp;
        setUnion(P, X, &U);
        int pivot = findPivot(graph, n, &U, degrees);
        
        getNeighbors(graph, pivot, &np);
        setNegation(&np, &nnp);
        setIntersection(P, &nnp, &pcopy);

        while (!isEmpty(&pcopy)) {
            int v = popFirstBit(&pcopy);
            set ni, P1, R1, X1;

            getNeighbors(graph, v, &ni);

            setIntersection(P, &ni, &P1);
            setBit(R, v, &R1);
            setIntersection(X, &ni, &X1);

            help(graph, n, &P1, &R1, &X1, degrees);

            unsetBit(P, v, P);
            setBit(X, v, X);
        }
    }

    void findMaxClique(int graph[N_MAX][N_MAX], int n) {

        set P = {_0};
        set X = {_0};
        set R = {_0};

        for (int i=0; i < n; i++) setBit(&P, i, &P);

        toNotDirectedGraph(graph, n);
        set* graphB = (set*) malloc (N_MAX*sizeof(set));
        toBitGraph(graph, graphB, n);

        int degrees [N_MAX] = {0};
        for (int i=0; i < n; i++) { degrees[i] = popcount(graphB[i]); }

        help (graphB, n, &P, &R, &X, degrees);

        printf("Clique Members: ");
        printSet(ans);
        printf("Size: %d\n", popcount(ans));

        free(graphB);
    }
