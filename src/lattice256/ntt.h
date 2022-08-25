//
// Created by jayamine on 8/28/21.
//

#ifndef CIP_SIS_NTT_H
#define CIP_SIS_NTT_H

#if(ENABLE_56INT == 0)
#include "structs32.h"
#else
#include "structs56.h"
#endif

zint_t montgomery_reduce(zint2_t a);

void ntt(zint_t a[N]);

void invntt_tomont(zint_t a[N]);

void invntt(zint_t a[N]);

#endif //CIP_SIS_NTT_H
