#include <iostream>

#define ROSE_ASSERT(x) assert(x)
#include "../GenericIntervalLattice.h"
#include "../NumberIntervalLattice.h"
using namespace std;

int main() {
  {
    GenericIntervalLattice<int> i1(3,5);
    GenericIntervalLattice<int> i2(2,4);
    cout<<i1.toString()<<endl;
    cout<<i2.toString()<<endl;
    GenericIntervalLattice<int> i3=GenericIntervalLattice<int>::join(i1,i2);
    cout<<i3.toString()<<endl;
    GenericIntervalLattice<int> i4=GenericIntervalLattice<int>::meet(i1,i2);
    cout<<i4.toString()<<endl;
    GenericIntervalLattice<int> i6=GenericIntervalLattice<int>::highInfInterval(7);
    cout<<i6.toString()<<endl;
    GenericIntervalLattice<int> i7=GenericIntervalLattice<int>::lowInfInterval(6);
    cout<<i7.toString()<<endl;
    GenericIntervalLattice<int> i8=GenericIntervalLattice<int>::join(i6,i7);
    cout<<i8.toString()<<endl;
    GenericIntervalLattice<int> i9=GenericIntervalLattice<int>::meet(i6,i7);
    cout<<i9.toString()<<" isEmpty:"<<i9.isEmpty()<<endl;
  }
  cout<<"---------------------- ARITH TEST -----------------"<<endl;
  {
    GenericIntervalLattice<int> i1(3,5);
    GenericIntervalLattice<int> i10=i1;
    cout<<i10.toString()<<endl;
    i10.arithAdd(3);
    cout<<i10.toString()<<endl;
    i10.arithSub(3);
    cout<<i10.toString()<<endl;
    i10.arithMul(3);
    cout<<i10.toString()<<endl;
    i10.arithDiv(3);
    cout<<i10.toString()<<endl;
    i10.arithMod(2);
    cout<<i10.toString()<<endl;
  }
  cout<<"---------------------- ARITH TEST -----------------"<<endl;
  {
    GenericIntervalLattice<int> i1(3,5);
    GenericIntervalLattice<int> i10=i1;
    i10=GenericIntervalLattice<int>(1,3);
    cout<<i10.toString()<<endl;
    GenericIntervalLattice<int> i11=GenericIntervalLattice<int>(2,5);
    cout<<i11.toString()<<endl;
    i10.arithAdd(i11);
    cout<<i10.toString()<<endl;
    i10.arithMul(i11);
    cout<<i10.toString()<<endl;
    i10.arithDiv(i11);
    cout<<i10.toString()<<endl;
  }
  cout<<"---------------------- SUBINTERVAL TEST -----------------"<<endl;
  {
    GenericIntervalLattice<int> i30(1,3);
    GenericIntervalLattice<int> i31(-1,4);
    GenericIntervalLattice<int> i32(1,4);
    //GenericIntervalLattice<int> i33();
    cout<<"Test1:(1):"<<GenericIntervalLattice<int>::isSubIntervalOf(i30,i32)<<endl;
    cout<<"Test2:(0):"<<GenericIntervalLattice<int>::isSubIntervalOf(i32,i30)<<endl;
    cout<<"Test3:(1):"<<GenericIntervalLattice<int>::isSubIntervalOf(i32,i31)<<endl;
    cout<<"Test3:(1):"<<GenericIntervalLattice<int>::isSubIntervalOf(i32,i32)<<endl;
    cout<<"Length:"<<i30.length()<<endl;
    cout<<"Length:"<<i31.length()<<endl;
    cout<<"Length:"<<i32.length()<<endl;
  }  
  cout<<"---------------------- SUBINTERVAL TEST -----------------"<<endl;
  {
    GenericIntervalLattice<int> i30(1,3);
    GenericIntervalLattice<int> i31(4,7);
    GenericIntervalLattice<int> i32(1,4);
    GenericIntervalLattice<int> i33(-1,1);
    GenericIntervalLattice<int> i34(3,3);
    GenericIntervalLattice<int> i35(3,3);
    GenericIntervalLattice<int> i36(4,4);
    cout<<"Test1:(true):"<<GenericIntervalLattice<int>::isSmaller(i30,i31).toString()<<endl;
    cout<<"Test2:(top):"<<GenericIntervalLattice<int>::isSmaller(i32,i31)<<endl;
    cout<<"Test3:(top):"<<GenericIntervalLattice<int>::isSmaller(i32,i30)<<endl;
    cout<<"Test4:(top):"<<GenericIntervalLattice<int>::isSmaller(i33,i30)<<endl;
    cout<<"Test5:(false):"<<GenericIntervalLattice<int>::isSmaller(i34,i35)<<endl;
    cout<<"Test6:(false):"<<GenericIntervalLattice<int>::isSmaller(i34,i34)<<endl;
    cout<<"Test6:(true):"<<GenericIntervalLattice<int>::isSmaller(i35,i36)<<endl;

  }

  cout<<"---------------------- INT LATTICE TEST -----------------"<<endl;
  {
    using namespace CodeThorn;
    NumberIntervalLattice i1(3,5);
    NumberIntervalLattice i2(2,4);
    i1.isTop();
    cout<<i1.toString()<<endl;
    cout<<i2.toString()<<endl;
    NumberIntervalLattice i3=NumberIntervalLattice::join(i1,i2);
    cout<<i3.toString()<<endl;
    NumberIntervalLattice i4=NumberIntervalLattice::meet(i1,i2);
    cout<<i4.toString()<<endl;
    NumberIntervalLattice i6=NumberIntervalLattice::highInfInterval(7);
    cout<<i6.toString()<<endl;
    NumberIntervalLattice i7=NumberIntervalLattice::lowInfInterval(6);
    cout<<i7.toString()<<endl;
    NumberIntervalLattice i8=NumberIntervalLattice::join(i6,i7);
    cout<<i8.toString()<<endl;
    NumberIntervalLattice i9=NumberIntervalLattice::meet(i6,i7);
    cout<<i9.toString()<<" isEmpty:"<<i9.isEmpty()<<endl;
  }

  return 0;
}
