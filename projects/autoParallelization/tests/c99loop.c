/*
Contributed by Jeff Keasler
Liao, 10/22/2009
*/
int main(int argc,char *argv[])
{
  double a[20UL][20UL];
  for (int i = 0; i <= 19 - 1; i += 1) {
    for (int j = 0; j <= 20 - 1; j += 1) {
      a[i][j] += a[i + 1][j];
    }
  }
  return 0;
}

// with shadow i and j
void foo (int i, int j)
{
  double a[20][20];
  for (int i = 0; i <= 19 - 1; i += 1) {
    for (int j = 0; j <= 20 - 1; j += 1) {
      a[i][j] += a[i + 1][j];
    }
  }

}


