void foobar()
  {
 // This is an assignment initializer using the "default value" for type int (zero is the default value for int so we get zero internally).
    int z = int();

#if 1
    int y;

    typedef int xxx;

    y.~xxx();

    int* y_ptr = &y;

 // Call the integer destructor!
 // y_ptr->~int();

 // unparses as: "y_ptr -> ~int;" but should unparse as "typedef int xxx; y_ptr->~xxx();"
    y_ptr->~xxx();
#endif
  }
