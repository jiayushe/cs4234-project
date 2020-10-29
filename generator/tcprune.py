import sys
import os

for fn in os.listdir(sys.argv[1]):
    fp = sys.argv[1] + fn
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
        
        if e > 600000:
            f.close()
            os.remove(fp)

            

