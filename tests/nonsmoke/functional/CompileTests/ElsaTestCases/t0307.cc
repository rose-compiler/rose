// t0307.cc
// 5.16 para 1: conversion applied to first argument of ?:

struct A {
  operator bool ();
};

struct B {};


void f(A const &aref)
{
  A a;
  a? 1 : 2;                   // ok

  B b;
  //ERROR(1): b? 3 : 4;       // no conversion operator

  //ERROR(2): aref? 5 : 6;    // operator not usable b/c of 'const'
  
  A const *aptr;
  aptr? 7 : 8;                // pointer-to-bool conversion ok
}




