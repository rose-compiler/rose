#include <sys/time.h>

int main() {
  struct timeval val;
  struct timezone zone;
  gettimeofday(&val,&zone);
  return 0;
}
