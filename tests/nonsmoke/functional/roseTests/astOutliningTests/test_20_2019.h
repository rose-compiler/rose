//comments here
//
class A {

  public: 
  // This constructor causes the tool go process the generated file to generate rose_rose_xxx_lib_lib.cpp
   A (double axxIn, double axyIn, double axzIn, double ayxIn, double ayyIn, double ayzIn, double abxIn, double abyIn, double abzIn)
   {
     axx = axxIn; axy = axyIn; axz = axzIn; ayx = ayxIn; ayy = ayyIn; ayz = ayzIn; 
#pragma rose_outline
     {
       abx = abxIn; aby = abyIn; abz = abzIn; 
     }
   }

  double axx, axy, axz, ayx, ayy, ayz, abx, aby, abz; 

};
