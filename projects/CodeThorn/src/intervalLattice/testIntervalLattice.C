#include <iostream>
#include "../IntervalLattice.h"

using namespace std;

int main() {
  IntervalLattice<int> i1(3,5);
  IntervalLattice<int> i2(2,4);
  cout<<i1.toString()<<endl;
  cout<<i2.toString()<<endl;
  IntervalLattice<int> i3=IntervalLattice<int>::join(i1,i2);
  cout<<i3.toString()<<endl;
  IntervalLattice<int> i4=IntervalLattice<int>::meet(i1,i2);
  cout<<i4.toString()<<endl;
  IntervalLattice<int> i6=IntervalLattice<int>::highInfInterval(7);
  cout<<i6.toString()<<endl;
  IntervalLattice<int> i7=IntervalLattice<int>::lowInfInterval(6);
  cout<<i7.toString()<<endl;
  IntervalLattice<int> i8=IntervalLattice<int>::join(i6,i7);
  cout<<i8.toString()<<endl;
  IntervalLattice<int> i9=IntervalLattice<int>::meet(i6,i7);
  cout<<i9.toString()<<" isEmpty:"<<i9.isEmpty()<<endl;
  cout<<"---------------------- ARITH TEST -----------------"<<endl;
  IntervalLattice<int> i10=i1;
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
  cout<<"---------------------- ARITH TEST -----------------"<<endl;
  {
    i10=IntervalLattice<int>(1,3);
    cout<<i10.toString()<<endl;
    IntervalLattice<int> i11=IntervalLattice<int>(2,5);
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
    IntervalLattice<int> i30(1,3);
    IntervalLattice<int> i31(-1,4);
    IntervalLattice<int> i32(1,4);
    IntervalLattice<int> i33();
    cout<<"Test1:(1):"<<IntervalLattice<int>::isSubIntervalOf(i30,i32)<<endl;
    cout<<"Test2:(0):"<<IntervalLattice<int>::isSubIntervalOf(i32,i30)<<endl;
    cout<<"Test3:(1):"<<IntervalLattice<int>::isSubIntervalOf(i32,i31)<<endl;
    cout<<"Test3:(1):"<<IntervalLattice<int>::isSubIntervalOf(i32,i32)<<endl;
    cout<<"Length:"<<i30.length()<<endl;
    cout<<"Length:"<<i31.length()<<endl;
    cout<<"Length:"<<i32.length()<<endl;
  }  
  cout<<"---------------------- SUBINTERVAL TEST -----------------"<<endl;
  {
    IntervalLattice<int> i30(1,3);
    IntervalLattice<int> i31(4,7);
    IntervalLattice<int> i32(1,4);
    IntervalLattice<int> i33(-1,1);
    IntervalLattice<int> i34(3,3);
    IntervalLattice<int> i35(3,3);
    IntervalLattice<int> i36(4,4);
    cout<<"Test1:(true):"<<IntervalLattice<int>::isSmaller(i30,i31).toString()<<endl;
    cout<<"Test2:(top):"<<IntervalLattice<int>::isSmaller(i32,i31)<<endl;
    cout<<"Test3:(top):"<<IntervalLattice<int>::isSmaller(i32,i30)<<endl;
    cout<<"Test4:(top):"<<IntervalLattice<int>::isSmaller(i33,i30)<<endl;
    cout<<"Test5:(false):"<<IntervalLattice<int>::isSmaller(i34,i35)<<endl;
    cout<<"Test6:(false):"<<IntervalLattice<int>::isSmaller(i34,i34)<<endl;
    cout<<"Test6:(true):"<<IntervalLattice<int>::isSmaller(i35,i36)<<endl;

  }
  return 0;
}
