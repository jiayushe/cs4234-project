import random
import os

N = 10
E = 20
W = 10
T = 10

for t in range(T):
    n = 10
    c = random.randint(1, n // 5)
    q = n // c
    vc = n - c
    r = n % (n - vc)
    k = r*(q+1)*q/2 + (n-vc-r)*q*(q-1)/2
    te = n*(n-1)/2
    lb = te - vc*(vc-1)/2 + vc*(n-vc)
    ub = te - k
    e = int(ub)
    rr = random.randint(0, q - 1)
    seed = t
    f = f"../test-cases/extra-small-{t}.sanchis"
    w = random.randint(1, W)
    print("Testing ", n, e, c, rr)
    os.system(f"./gen {n} {e} {c} {rr} {seed} {f}")
    os.system(f"python3 dimacs_to_mpf.py {f} {w}")

