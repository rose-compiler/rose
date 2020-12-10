// DQ (7/21/2020): Coroutine support is not available in EDG 6.0 or the headers are not available in GNU 6.1.

lazy<int> f() {
  co_return 7;
}

