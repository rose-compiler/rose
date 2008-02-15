// g0033.cc
// does typeof applied to an array type incorporate knowledge
// of a later-added size?
        

extern int a[];                 // initially, no size specified

extern typeof(a) b;             // int b[];

void f1()
{
  //ERROR(1): sizeof(a);        // invalid, size unknown
  //ERROR(2): sizeof(b);        // same
}

int a[5];

void f2()
{
  sizeof(a);                    // valid, size now known
  //ERROR(3): sizeof(b);        // valid? NO
}

int b[10];                      // give 'b' a different size

void f3()
{
  sizeof(b);                    // now ok
}


// EOF
