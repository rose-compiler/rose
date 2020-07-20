generator<int> iota(int n = 0) {
  while(true)
    co_yield n++;
}

