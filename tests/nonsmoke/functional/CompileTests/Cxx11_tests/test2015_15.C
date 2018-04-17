
#define BLOCKSIZE 32

int foo();

void foobar()
   {
  // DQ (2/7/2015): Fixup for error yet to be fixed in ROSE (or fixed on alternative branch not yet merged).
  // Point hi = getOnes()*(BLOCKSIZE-1);
     const int adjustedBlockSize = BLOCKSIZE-1;
     const int hi = foo() * adjustedBlockSize;

   }
