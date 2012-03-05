// All pragmas here are ignored as errors:

#pragma skel initializer repeat(3)
float foobar_b [] = {5.0,6.0};

#pragma skel initializer repeat(3)
int foobar_c [] = {5,6};

int main () {

  #pragma skel loop iterate exactly(12)
  return 1;
}
