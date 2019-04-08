#!/usr/bin/env python3

import threading
import time
import random

def hello(n):
    time.sleep(random.randint(1,3))
    print("[{0}] Hello!".format(n))

threads = [ ]
for i in range(10):
    t = threading.Thread(target=hello, args=(i,))
    threads.append(t)
    t.start()

for one_thread in threads:
    one_thread.join()

print("Done!")