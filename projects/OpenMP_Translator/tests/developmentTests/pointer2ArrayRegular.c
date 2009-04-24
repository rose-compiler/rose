// regular array types can use &array  
//

static void makea ()
{
  static int colidx[100];
  int i,j;
#pragma omp parallel for private(i)
  for (i = 1; i <= 100; i++) 
    colidx[i] = 0;
}


int main()
{
  makea();
}

