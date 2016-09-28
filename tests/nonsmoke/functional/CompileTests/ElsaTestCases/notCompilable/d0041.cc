// modified version of d0040.cc where I attempt to explicitly put in
// the implicit copy ctor; g++ won't compile this
// NOTE: this is not legal C++
struct ip_opts {
  char ip_opts[40];
  ip_opts(ip_opts const &other)
    : ip_opts(other.ip_opts)
  {}
};
