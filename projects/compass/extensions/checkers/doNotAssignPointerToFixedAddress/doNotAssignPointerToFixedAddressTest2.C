// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2:foldmethod=syntax
// test MITRE CWE-587, assignment of a fixed address to a pointer


// test for assignments in parameter defaults
int test(int* x = (int*) 0x07f00020) {
  return 0;
}


int main() {

  return test();
}

/* Expected output:
 
 DoNotAssignPointerToFixedAddress: doNotAssignPointerToFixedAddressTest2.C:7.10: Pointer x is initialized to a fixed address.

*/
