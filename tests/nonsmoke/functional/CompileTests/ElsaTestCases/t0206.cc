// t0206.cc
// Daniel's ptr-to-memberI.cc

// illegal pointer to member declarations
class A;

int main() {
  int A::*q;
  //ERROR(1): int & A::*q2;                 // illegal
}
