// DQ (7/21/2020): This does not appear to be supported in EDG 6.0.

// #include<socket.h>

typedef unsigned long size_t;

task<> tcp_echo_server() {
  char data[1024];
  for (;;) {
    size_t n = co_await socket.async_read_some(buffer(data));
    co_await async_write(socket, buffer(data, n));
  }
}

