#!/usr/bin/env python3

import multiprocessing
import time
import random
import os

mylist = [ ]

def hello(n):
    time.sleep(random.randint(1,3))
    mylist.append(os.getpid())
    print("[{0}] Hello!".format(n))

processes = [ ]
for i in range(10):
    t = multiprocessing.Process(target=hello, args=(i,))
    processes.append(t)
    t.start()

for one_process in processes:
    one_process.join()

print("Done!")
print(len(mylist))
print(mylist)
