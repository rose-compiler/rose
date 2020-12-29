#!/usr/bin/env python3

import threading
import time
import random

def hello(n):
    time.sleep(random.randint(1,3))
    print("[{0}] Hello!".format(n))

for i in range(10):
    threading.Thread(target=hello, args=(i,)).start()

print("Done!")
