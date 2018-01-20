#include <cstdlib>
using namespace std;

int main() {
  int p;
  int* a[3];
  int* intp;
  int* intp2;
  int** pa;
  void* mpvoid;

  p=1000;
  intp=&p;
  //intp2=&*intp;
  //pa=&intp;
  //  **pa=7000;
  //  **pa=**pa+1;
  return 0;
}
