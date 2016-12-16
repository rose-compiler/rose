#include <fstream>
void write_array(int * A, int N, std::ofstream & ofs)
{
  for (int i=0; i<N; ++i)
  {
    ofs<<A[i]<< "";
  }
}
