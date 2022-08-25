//
// Modified from Dilitihium https://github.com/pq-crystals/dilithium which is a public repository
//

#include "ntt.h"


/**
 * montgomery_reduce
 * @param a
 * @return  t \equiv a*2^{-64} (mod Q) such that -Q < t < Q
 */
zint_t montgomery_reduce(zint2_t a) {
    zint_t t;

    //t = (zint_t)((zint2_t)(a*QINV) & FILTER);
    //t = (zint_t)(a - (zint2_t)t*Q) >> KMONT;

    t = (zint_t)((zint2_t)a*QINV & FILTER);
    t = (zint_t)((a - (zint2_t)t*Q) >> KMONT);

    return t;
}


void ntt(zint_t a[N]) {
    unsigned int len, start, j, k;
    zint_t zeta, t;

    k = 0;
    for(len = 128; len > 0; len >>= 1) {
        for(start = 0; start < N; start = j + len) {
            zeta = zetas[++k];
            for(j = start; j < start + len; ++j) {
                t = montgomery_reduce((zint2_t)zeta * a[j + len]);
                a[j + len] = a[j] - t;
                a[j] = a[j] + t;
            }
        }
    }
}


void invntt_tomont(zint_t a[N]) {
    unsigned int start, len, j, k;
    zint_t t, zeta;

    k = 256;
    for(len = 1; len < N; len <<= 1) {
        for(start = 0; start < N; start = j + len) {
            zeta = -zetas[--k];
            for(j = start; j < start + len; ++j) {
                t = a[j];
                a[j] = t + a[j + len];
                a[j + len] = t - a[j + len];
                a[j + len] = montgomery_reduce((zint2_t)((zint2_t)zeta * (zint2_t)a[j + len]));
            }
        }
    }

    for(j = 0; j < N; j++) {
        a[j] = montgomery_reduce((zint2_t)F1 * a[j]);
    }
}

void invntt(zint_t a[N]) {
    unsigned int start, len, j, k;
    zint_t t, zeta;

    k = 256;
    for(len = 1; len < N; len <<= 1) {
        for(start = 0; start < N; start = j + len) {
            zeta = -zetas[--k];
            for(j = start; j < start + len; ++j) {
                t = a[j];
                a[j] = t + a[j + len];
                a[j + len] = t - a[j + len];
                a[j + len] = montgomery_reduce( ((zint2_t)zeta * (zint2_t)a[j + len]));
            }
        }
    }

    for(j = 0; j < N; ++j) {
        a[j] = montgomery_reduce((zint2_t)F2 * a[j]);
    }
}


