void foo()
{
  int arr[4];
  int (*p1_arr)[4];
  p1_arr = &arr;
  *p1_arr[3] = 0;
  int **p;
  p = new int*[10];
  p[0] = new int[10];
  p[0][4] = 4;
}

