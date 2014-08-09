#include <iostream>
#include "IntervalLattice.h"

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
  return 0;
}
