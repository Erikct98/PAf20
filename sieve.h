#ifndef PAF20_PARALLELALGORITHMS_SIEVE_H
#define PAF20_PARALLELALGORITHMS_SIEVE_H

#include <bsp.h>
#include <stdio.h>
#include <stdlib.h>
#include "edupack/bspedupack.h"

static unsigned long S = 1l * 1000l * 1000l * 1000l;

/*
 * Returns the integer closest to the square root of x
 * This is the integer below or above, depending on getFloor
 * Invariant: start <= sqrt(x) <= end
 */
static long intSqrt(unsigned long x, bool getFloor){
    // Base cases
    if (x == 0 || x == 1)
        return x;

    long start = 0, end = x;
    while (start + 1 < end) {
        long center = (start + end)/2;
        if (center * center > x){
            end = center;
        } else {
            start = center;
        }
    }
    return getFloor ? start : end;
}

long min(long one, long two){
    return one < two ? one : two;
}

long max(long one, long two){
    return one > two ? one : two;
}

long gcd(long one, long two){
    if (two == 0) return one;
    return gcd(two, one % two);
}

long lcm(long one, long two){
    return one * two / gcd(one, two);
}

long ceildiv(long n, long d){
    return (n + d - 1) / d;
}

#endif //PAF20_PARALLELALGORITHMS_SIEVE_H
