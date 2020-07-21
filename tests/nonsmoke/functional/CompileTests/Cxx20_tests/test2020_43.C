// DQ (7/21/2020): Coroutine support is not available in EDG 6.0 or the headers are not available in GNU 6.1.

generator<int> iota(int n = 0) {
  while(true)
    co_yield n++;
}

