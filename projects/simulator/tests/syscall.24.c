#include <sys/types.h>
#include <unistd.h>

int main() {
  int result = getuid();
  
  if( result == -1 )
    err(1,"getuid failed");
  return 0;
}
