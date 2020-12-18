#!/usr/bin/env python3

import multiprocessing
import time
import random

def hello(n):
    time.sleep(random.randint(1,3))
    print("[{0}] Hello!".format(n))

processes = [ ]
for i in range(10):
    t = multiprocessing.Process(target=hello, args=(i,))
    processes.append(t)
    t.start()

for one_process in processes:
    one_process.join()

print("Done!")
