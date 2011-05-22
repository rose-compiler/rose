#define SWAP(X,Y) \
{ \
  int _T = X; \
  X=Y; \
  Y=_T; \
}

int sort(int* a, int n) {
  int i,j;
  for (i = 0; i < n; i++) {
    for (j = 0; j < i; j++) {
      if (a[i] > a[j]) SWAP(a[i],a[j]);
    }
  }
}

int main()
{
  int a[100];
  sort(a, 100);
}

