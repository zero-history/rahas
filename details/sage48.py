Q = 100433627766186892221372630771322662657637687111424552202241 #2**K1 - 2**K2 + 1
K1 = 196
K2 = 12
N = 256
n = 9 * N
m = 5 * N
delta = 1.004
GAMMA2 = 2**130
GAMMA1 = 2**120

ROOT = 591383877579754311354681575070039576113163592199716231148
MONT = 67092480 # 2**210 in [0, Q]
QINV = 848369132375585841925902541647579249076303725019922490334384129 # Q^(-1) mod 2^210
f = 17583597158400
f2 = 262080

print("\nBasic details ================")
print("Q:", Q)
print("Is Q prime:", is_prime(Q))

print("Lattice Reduction Cost for Delta =", delta, ":", lattice_reduction_cost(BKZ.qsieve, delta, (n+m)/2, log(Q)))

print("Infinity Norm (GAMMA1): (log 2)",  log(GAMMA1, 2))
print("Estimated nfinity Norm (GAMMA1) for Delta = ", delta, ": (log 2)", log(RR(delta**(n+m) * (Q)**(n/(n+m))),2))
print("Is GAMMA1 < estimated norm:", log(RR(delta**(n+m) * (Q)**(n/(n+m))),2) > log(GAMMA1, 2))

print("\nMontgomery details ================")

print("Montgomery coef: ", MONT)
print("gcd with Q:", gcd(MONT, Q) == 1)
print("Q inverse mod MONT:", QINV)
print("test manually: print(\"Valid QINV:\", QINV == inverse_mod(Q, R))")
print("GCD QINV:", 1 == gcd(QINV, MONT))

print("\nNTT details ================")

Zp = Integers(Q)
print("Root of unity: ", ROOT)
print("Smallest Root of Unity:", ROOT == min(Zp.zeta(N*2, all=True)))

print("f:", f)
print("valid:", f == (MONT * MONT * inverse_mod(256, Q)) % Q)

print("f2:", f2)
print("valid:", f2 == (MONT * inverse_mod(256, Q)) % Q)

def montgomery_reduce(a) :
    filter = 316912650057057350374175801343
    t = (a * QINV) & filter
    t = (a - ((t * Q) & (2**128 - 1))) >> 98
    return t

