import random
import os

N = 4000
E = 600000
W = 10**7
T = 100

for t in range(T):
    n = random.randint(10, N)
    c = random.randint(1, n // 5)
    q = n // c
    vc = n - c
    r = n % (n - vc)
    k = r*(q+1)*q/2 + (n-vc-r)*q*(q-1)/2
    te = n*(n-1)/2
    lb = te - vc*(vc-1)/2 + vc*(n-vc)
    ub = te - k
    e = random.randint(min(lb, ub, E), min(max(lb, ub), E))
    rr = random.randint(0, q - 1)
    seed = t
    f = f"../test-cases/{t}.sanchis"
    w = random.randint(1, W)
    print("Testing ", n, e, c, rr)
    os.system(f"./gen {n} {e} {c} {rr} {seed} {f}")
    os.system(f"python3 dimacs_to_mpf.py {f} {w}")

