//
// Created by jayamine on 1/2/22.
//

#include <boost/multiprecision/cpp_int.hpp>
using namespace boost::multiprecision;
using namespace std;

#define N       256
#define K1      283
#define K11     282
#define K2      91
#define KMONT   285


static const int512_t Q("15541351137805832567355695254588151253139254712417116170012023397832663521091869736961");
static const int512_t Q2("7770675568902916283677847627294075626569627356208558085006011698916331760545934868480");
// Zp = Integers(Q), min(Zp.zeta(N*2, all=True))
static const int512_t ROOT("110047119455493901099475395281769776735154522876170144943517083202581617091636349264");
// R = 2**244
static const int512_t R("62165404551223330269422781018352605012557018849668464680057997111644937126566671941632");
// MONT = R in [0, Q]
static const int512_t MONT("9903520314283042199192993788");
// inverse_mod(Q, R)
static const int512_t QINV("46639230514138011210433644059917642483039371496114876738164354734997833751691702304769");
// R - 1
static const int512_t FILTER("62165404551223330269422781018352605012557018849668464680057997111644937126566671941631");
// MONT^2/256 (MONT*MONT*inverse_mod(256, Q))% Q
static const int512_t F1("14570016691692968031895964301176774923703267765105635996142750027754205390735448997889");
// MONT/256  (MONT*inverse_mod(256, Q))% Q
static const int512_t F2("242833611528216133864932738352939863330300854881517440156476551217363035650651062272");

int1024_t power_mod(int1024_t a, int1024_t b) {
    int1024_t res = 1;
    a = a % Q;
    for(int i=1 ; i<=b ; ++i)
        res = (res * a) % Q;

    return res;
}

int1024_t bit_reversal(int1024_t a) {
    int1024_t n_ = 0;
    for (int i = 0; i < 8; i++) {
        n_ = ((a & 1) << (7 - i)) | n_;
        a = a >> 1;
    }
    return n_;
}

int1024_t reduce(int1024_t a) {
    int1024_t t = (a + ((int512_t)1 << (K1 - 1))) >> K1;
    t = a - t*Q;
    return t;
}

int1024_t reduce_extact(int1024_t a) {
    int1024_t t = (a + ((int512_t)1 << (K1 - 1))) >> K1;
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
    int1024_t zeta;
    int1024_t zeta_extact;
    cout << "{";
    for (int i = 0; i < N; i++) {
        zeta = reduce((MONT * power_mod(ROOT, bit_reversal(i))) % Q);
        cout << "(int512_t)(\"" << zeta << "\"),";
    }
    cout << "}\n";
    return 0;

}



