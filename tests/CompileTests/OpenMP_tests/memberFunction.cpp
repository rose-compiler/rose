#include <iostream>
using namespace std;
class A
{
  private:
    int i;
  public:
    void pararun()
    {
#pragma omp parallel
      {
#pragma omp critical
        cout<<"i= "<< i <<endl;
      }
    }
};

