/*
 * A test for data-sharing attribute of static data members 
 * */
#include <iostream>
using namespace std;

class A
{
  private:
    int i;
  public:
    // static data member
    static int counter; 
    A() {i=100;};

    void pararun()
    {
#pragma omp parallel
      {
#pragma omp single private(i)
        {
          counter++;
          cout<<"i= "<< i <<endl;
        }
      }
    }
};

int A::counter=0;

int main()
{
  A a;
  a.pararun();
  return 0;
}

