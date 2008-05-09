
using namespace std;

int main(int argc, char** argv) {

  int x=5;

  int* k ;
  int** y;

  k=&x;
  y=&k;

  int z = *k;
  z = **y;
  
  k=0;
  x=**y;
}
