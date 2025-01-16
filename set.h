#ifndef SHALOM
#define SHALOM

#define uchar unsigned char
#define ull unsigned long long int
#define N_MAX 128
#define ULL_SIZE 64
#define N 20
#define I 1

typedef struct set {
    // a[0] is lower, a[1] is higher
    ull a[2];
} set;

typedef void (*algo)(uchar*, int, set, set, set);
typedef void (*stupid)(uchar, int, int*, int, int, int, int *, int *);

set setUnion(set A, set B);

set setIntersection(set A, set B);

uchar getBit(set A, int i);

set setBit(set A, int i);

set unsetBit(set A, int i);

uchar isEmpty(set A);

uchar popcount(set A);

uchar popBit(set* A, int i);

uchar popFirstBit(set* A);

set compareSets (set A, set B);


#endif