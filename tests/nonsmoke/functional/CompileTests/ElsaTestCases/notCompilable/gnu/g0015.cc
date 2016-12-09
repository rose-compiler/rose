// t0001.cc
// hex floating literals


void f(float);

void foo()
{
  // optional parts:         HEX1 "." HEX2 sign suffix
  
  // everything
  f(0x1aE.2bFp+312f);   //    *    *   *    *     *

  // missing one
  f(0x.2bFp+312f);      //         *   *    *     *
  f(0x1aE2bFp+312f);    //    *             *     *
  f(0x1aE.p+312f);      //    *    *        *     *
  f(0x1aE.2bFp312f);    //    *    *   *          *
  f(0x1aE.2bFp+312);    //    *    *   *    *

  // variations
  f(0X1aE.2bFP-312F);   //    *    *   *    *     *
  f(0x1aE.2bFP+312l);   //    *    *   *    *     *
  f(0X1aE.2bFP-312L);   //    *    *   *    *     *

  // missing more
  f(0x.2bFp312f);       //         *   *          *
  f(0x.2bFp+312);       //         *   *    *
  f(0x1aE2bFp312f);     //    *                   *
  f(0x1aE2bFp+312);     //    *             *
  f(0x1aE2bFp312);      //    *
  f(0x1aE.p312f);       //    *    *              *
  f(0x1aE.p+312);       //    *    *        *     *

  // errors
  //ERROR(1): f(0x1aE.2bFp);      // need digits after 'p'
  //ERROR(2): f(0x1aE.2bF);       // must have 'p'
}

// EOF
