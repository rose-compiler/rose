void foobar( int x[100] );

static int xArray[100];

void foo()
   {
      int it;
/*-------------------------------------------------------------------
c---->
c  Do one iteration untimed to init all code and data page tables
c---->                    (then reinit, start timing, to niter its)
c-------------------------------------------------------------------*/

    for (it = 1; it <= 1; it++) {
       it;
       foobar(xArray);
       it;
    }

   }
