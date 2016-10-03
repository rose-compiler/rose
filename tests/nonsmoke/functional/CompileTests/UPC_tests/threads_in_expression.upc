
// \pp 03\25\11
// tests the fidelity of translating THREADS

shared[4] int X[THREADS*4];
shared    int Y[THREADS];
// shared    int Z[2 * THREADS+1]; /* not yet supported */

int main()
{
  int x = 2 * THREADS;
  int y = THREADS;
  int z = 2 * THREADS + 1;
}
