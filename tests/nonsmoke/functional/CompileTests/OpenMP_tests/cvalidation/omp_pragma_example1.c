#if 0
//inputBug342-2.c
// -rose:C
// roseomp: main.C:1685: static SgStatement*
// ASTtools::getNextStatement(SgPragmaDeclaration*): Assertion (*i) != __null failed.

void foo()
{
  int i;
  double sum = 0.0;
// Extra block inserted around subcollection of statements in original block!
{
   for (i = 1; i <= 10; i++) { sum++; }
// a statement between the for loop and pragma will help
// sum++;    
#pragma omp single
  }
  sum++;
}
#endif




void foo (void)
   {
     int i;
     double sum=0.0;

  // for(i=1;i<=10;i++) sum++;
  // while (i <= 10) { sum++; }
  // { int x; }
     while (i <= 10) { sum++; }

  // a statement between the for loop and pragma will help
  // sum++;

#pragma omp single
     sum++;
   }

