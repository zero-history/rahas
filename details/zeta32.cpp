//
// Created by jayamine on 1/2/22.
//

#include <boost/multiprecision/cpp_int.hpp>
using namespace boost::multiprecision;
using namespace std;

#define     N       256

#define K1      165
#define K11     164
#define K2      38
#define KMONT   167


static const int256_t Q("46768052394588893382517914646921056628714963468289");
static const int256_t Q2("23384026197294446691258957323460528314357481734144");
// Zp = Integers(Q), min(Zp.zeta(N*2, all=True))
static const int256_t ROOT("92254331596159226941262752792244772216041349194");
// R = 2**KMONT
static const int256_t R("187072209578355573530071658587684226515959365500928");
// MONT = R in [0, Q]
static const int256_t MONT("1099511627772");
// inverse_mod(Q, R)
static const int256_t QINV("140309866174537524756265411299509345715689573318657");
// R - 1
static const int256_t FILTER("187072209578355573530071658587684226515959365500927");
// MONT^2/256 (MONT*MONT*inverse_mod(256, Q))% Q
static const int256_t F1("43845049119927087546110544986210857072255563726849");
// MONT/256  (MONT*inverse_mod(256, Q))% Q
static const int256_t F2("730750818665451459101842416358141509827966271488");


int512_t power_mod(int512_t a, int512_t b) {
    int512_t res = 1;
    a = a % Q;
    for(int i=1 ; i<=b ; ++i)
        res = (res * a) % Q;

    return res;
}

int512_t bit_reversal(int512_t a) {
    int512_t n_ = 0;
    for (int i = 0; i < 8; i++) {
        n_ = ((a & 1) << (7 - i)) | n_;
        a = a >> 1;
    }
    return n_;
}

int512_t reduce(int512_t a) {
    int512_t t = (a + ((int256_t)1 << (K1 - 1))) >> K1;
    t = a - t*Q;
    return t;
}

int512_t reduce_extact(int512_t a) {
    int512_t t = (a + ((int256_t)1 << (K1 - 1))) >> K1;
    t = a - t*Q;
    if (t > (Q - 1)/2) {
        t = -Q + 1;
    } else if (t < -(Q - 1)/2) {
        t = Q + t;
    }
    return t;
}

// THis program will print zeta values, so I can just copy them to the main program's ntt_impl.h
int main() {
    int512_t zeta;
    int512_t zeta_extact;
    cout << "{";
    for (int i = 0; i < N; i++) {
        zeta = reduce((MONT * power_mod(ROOT, bit_reversal(i))) % Q);
        cout << "(int256_t)(\"" << zeta << "\"),";
    }
    cout << "}\n";
    return 0;

}


