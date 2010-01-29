/*
 * A test for data-sharing clause applied on member variables
 * */
#include <iostream>
using namespace std;

class A
{
  private:
    int i;
  public:
    A() {i=100;};
    void pararun()
    {
#pragma omp parallel
      {
#pragma omp single private(i)
        cout<<"i= "<< i <<endl;
      }
    }
};

int main()
{
  A a;
  a.pararun();
  return 0;
}

