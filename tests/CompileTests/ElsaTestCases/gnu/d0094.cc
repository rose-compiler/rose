// /home/ballAruns/tmpfiles/./authconfig-4.3.4-1/dnsclient-Hfb9.i:3562:47:
// error: reprSize of a sizeless array

// you are allowed to take the size of a dynamically-sized array

typedef unsigned int size_t;
void f(size_t buf_len)
{
  unsigned char label[buf_len];
  sizeof(label);
}
