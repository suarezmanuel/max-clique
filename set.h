#ifndef SHALOM
#define SHALOM

#define uchar unsigned char
#define ull unsigned long long int
#define N_MAX 128
#define ULL_SIZE 64
#define N 20

typedef struct set {
    // 16 bits
    ull a[2];
} set;

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