import os

parmlist = ["-N 500 -k 15 -maxk 20 -mut 0.1 -muw 0.1 -beta 2 -t1 2 -t2 1",
            "-N 500 -k 15 -maxk 20 -mut 0.1 -muw 0.3 -beta 2 -t1 2 -t2 1",
            "-N 500 -k 15 -maxk 20 -mut 0.3 -muw 0.1 -beta 2 -t1 2 -t2 1",
            "-N 1000 -k 15 -maxk 20 -mut 0.1 -muw 0.1 -beta 2 -t1 2 -t2 1",
            "-N 1000 -k 15 -maxk 20 -mut 0.1 -muw 0.3 -beta 2 -t1 2 -t2 1",
            "-N 1000 -k 15 -maxk 20 -mut 0.3 -muw 0.1 -beta 2 -t1 2 -t2 1",
            "-N 2000 -k 15 -maxk 20 -mut 0.1 -muw 0.1 -beta 2 -t1 2 -t2 1",
            "-N 2000 -k 15 -maxk 20 -mut 0.1 -muw 0.3 -beta 2 -t1 2 -t2 1",
            "-N 2000 -k 15 -maxk 20 -mut 0.3 -muw 0.1 -beta 2 -t1 2 -t2 1"]

for i in range(0,9):
    os.system("benchmark.exe "+parmlist[i])
    os.system("Community.exe > "+ str(i)+".txt");
