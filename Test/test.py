import os

parmlist = [" -k 15 -maxk 20 -mut 0.1 -muw 0.1 -beta 2 -t1 2 -t2 1 -om 2 -on ",
            " -k 15 -maxk 20 -mut 0.1 -muw 0.3 -beta 2 -t1 2 -t2 1 -om 2 -on ",
            " -k 15 -maxk 20 -mut 0.3 -muw 0.1 -beta 2 -t1 2 -t2 1 -om 2 -on "]
filename = "res.txt"
f = open(filename,'w')
f.close()
for i in [500,1000,2000]:
    for j in [0,1,2]:
        f = open(filename,'a')
        f.write(str(i) + " " + str(j) + "\n")
        f.close()
        for k in [x * 0.01 for x in range(0, 51, 2)]:
            os.system("benchmark.exe "+ " -N " + str(i) + parmlist[j]+str(k*i));
            t = os.popen("Community-naive.exe").read().strip()
            d = os.popen("Community-improved.exe").read()
            f = open(filename,'a')
            f.write(t)
            f.write(" ")
            f.write(d)
            f.close()
f.close()
            