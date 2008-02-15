#if 0
//inputBug342-3.c
// -rose:C
// roseomp: main.C:3114: static int
// OmpMidend::transOmpFor(SgPragmaDeclaration*): Assertion isSgForStatement(forstmt) != __null failed.

int i;
int m;

void foo()
{
// Extra block inserted around subcollection of statements in original block!
{
//#pragma omp for
    for (i = 0; i < 10; i++) {
      m++;
    }
// If we uncomment this statement then we get proper blocking
// m++;
    
#pragma omp for
  }
  for (i = 1; i < 10; i++) {
    m++;
  }
}
#endif

int i,m;
void
foo ()
{

//#pragma omp for
for (i = 0; i < 10; i++)
   m++;

// If we uncomment this statement then we get proper blocking
// m++;
 
#pragma omp for
for (i = 1; i < 10; i++)
  m++;

}
