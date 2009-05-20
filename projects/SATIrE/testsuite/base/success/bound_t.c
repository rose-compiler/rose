/*
 * This example program was sto^H^H^Hborrowed by GB from
 * http://www.tidorum.fi/bound-t/example/brochure/example.html
 *
 * There, it is used to illustrate the capabilities and limitations of
 * Tidorum's Bound-T tool. For us, it might be interesting because we can
 * deal with pointer information, which they can't.
 *
 * The code is completely unchanged except for this comment and for the line
 * declaring x in main; in the original version, x is not initialized.
 */

typedef unsigned int uint;

void Foo (int num, uint *x);
void Solve (uint *x);
void Count (int u, uint *x);
int Ones (uint x);

void main (void)
{
   uint x = 1; /* GB: was not initialized */
   Foo (6, &x);
   Solve (&x);
}


void Foo (int num, uint *x)
{
   Count (num + 3, x);
}


void Count (int u, uint *x)
{
   for (; u > 0; u -= 2)
   {   *x = *x * u;
   }
}


void Solve (uint *x)
{
   int i, k;
   for (i = 0; i < 8; i++)
   {
      k = Ones (*x);
      if (k == 0) break;
      Count (k, x);
   }
}


int Ones (uint x)
{
   int v = 0;
   while (x)
   {
      if (x & 1) v ++;
      x >>= 1;
   }
   return v;
}
