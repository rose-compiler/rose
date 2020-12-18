#!/usr/bin/env python3

import multiprocessing
import time
import random
import os
from multiprocessing import Queue

q_1 = Queue()

def hello(q, n):
    time.sleep(random.randint(1,3))
    q.put(os.getpid())
    print("[{0}] Hello!".format(n))

processes = [ ]
for i in range(10):
    t = multiprocessing.Process(target=hello, args=(q_1, i,))
    processes.append(t)

for process in processes:
    process.start()

for process in processes:
    process.join()

mylist = [ ]
while not q_1.empty():
    mylist.append(q_1.get())

print("Done!")
print(len(mylist))
print(mylist)
