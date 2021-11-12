import sys
import ntpath
import random

n = 0
m = 0

fp = sys.argv[1]
W = int(sys.argv[2])
nfn = "../test-cases/" + ntpath.basename(fp) + ".in"

with open(fp, "r") as f:
    lines = f.readlines()
    lines = list(filter(lambda x: len(x.strip()) > 0, lines))
    li = 0
    #Ignore comments
    while(lines[li][0] == 'c'):
        li += 1

    p = lines[li].split()
    n = int(p[0])
    e = int(p[1])
    li += 1

    e = []

    v = 0
    while(li < len(lines)):
        l = lines[li].split()
        li += 1
        d = int(l[0])
        while(d > 0):
            e.append((v, l[d]))
            d -= 1
        v += 1

    with open(nfn, "w") as nf:
        nf.write(f"{n} {len(e)}\n")
        for i in range(n):
            w = int(random.uniform(1, W))
            nf.write(f"{w} ")
        nf.write("\n")
        for edge in e:
            nf.write(f"{edge[0]} {edge[1]}\n")
            

