/*
Hi Robert,
    Do you think that you detect the hiding of the global "x"
by the  "A::x" in the  preinitialization list of  class B?

       int x;
       class A { protected: int x; A(int x){} }
       class B : A(x) {}

*/

int x;
class A { protected: int x; A(int x){} }
class B : A(x) {}



