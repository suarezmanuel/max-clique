#include "set.h"

inline set setUnion(set A, set B) {
    return (set){A.a[0] | B.a[0], A.a[1] | B.a[1]};
}

inline set setIntersection(set A, set B) {
    return (set){A.a[0] & B.a[0], A.a[1] & B.a[1]};
}

inline uchar getBit(set A, int i) {
    return 1 & (A.a[i < ULL_SIZE] >> (i-ULL_SIZE*(i >= ULL_SIZE)));
}

inline set setBit(set A, int i) {
    return (set){A.a[0] | ((i >= ULL_SIZE) << (i-ULL_SIZE)), A.a[1] | ((i < ULL_SIZE) << i)};
}

inline set unsetBit(set A, int i) {
    return (set){A.a[0] & ~((i >= ULL_SIZE) << (i-ULL_SIZE)), A.a[1] & ~((i < ULL_SIZE) << i)};
}

inline uchar isEmpty(set A) {
    return A.a[0] == 0 && A.a[1] == 0;
}

uchar popcount(set A) {
    uchar count = 0;
    for (int i=0; !isEmpty(A) && i < N_MAX; i++) { 
        count += popBit(&A, i);
    }
    return count;
}

uchar popBit(set* A, int i) {
    uchar bit = getBit(*A, i);
    *A = unsetBit(*A, i);
    return bit;
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
    if (countA != countB) {
        int f = countA > countB;
        return (set) {A.a[0]*f + B.a[0]*(1-f), A.a[1]*f + B.a[1]*(1-f)};
    }

    // shouldnt return {0,0} because A != B from using X in algo
    set ans = {0,0};
    int f = 0;
    int c = 0;
    uchar a, b;

    // same popcount
    for (int i=0; !c && i < countA; i++) {
        a = popFirstBit(&A);
        b = popFirstBit(&B);
        f = (a < b);
        c = (a == b);
        set ans = {A.a[0]*f + B.a[0]*(1-f), A.a[0]*f + B.a[1]*(1-f)};
    }

    return ans;
}