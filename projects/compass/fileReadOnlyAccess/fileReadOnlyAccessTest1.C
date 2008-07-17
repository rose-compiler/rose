#include <fstream>
#include <stdio.h>

int main()
{
  FILE *f1 = fopen( "f1.txt", "w" );
  std::fstream f2( "f2.txt", std::ios::app | std::ios::out );

  FILE *f1r = fopen( "f1.txt", "r" );
  std::ifstream f2r( "f2.txt", std::ios::in );

  return 0;
}
