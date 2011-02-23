#include <time.h>
#include <err.h>

int main(int argc, char** argv) {
  time_t t = time(NULL);
  if( t == ((time_t)-1) )
    err(1,"time failed");
  return 0;
}
