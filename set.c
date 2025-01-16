#include "set.h"

inline set setUnion(set A, set B) {
    return (set){A.a[0] | B.a[0], A.a[1] | B.a[1]};
}

inline set setIntersection(set A, set B) {
    return (set){A.a[0] & B.a[0], A.a[1] & B.a[1]};
}

inline uchar getBit(set A, int i) {
    if (i < ULL_SIZE) {
        return 1ULL & (A.a[0] >> i);
    } else {
        return 1ULL & (A.a[1] >> (i-ULL_SIZE));
    }
}

inline set setBit(set A, int i) {
    if (i < ULL_SIZE) {
        A.a[0] |= (1ULL << i);
    } else {
        A.a[1] |= (1ULL << (i-ULL_SIZE));
    }
    return A;
}

inline set unsetBit(set A, int i) {
    if (i < ULL_SIZE) {
        A.a[0] &= ~(1ULL << i);
    } else {
        A.a[1] &= ~(1ULL << (i-ULL_SIZE));
    }
    return A;
}

inline uchar isEmpty(set A) {
    return A.a[0] == 0ULL && A.a[1] == 0ULL;
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
    if (countA > countB) {
        return A;
    } else if (countB < countA){
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