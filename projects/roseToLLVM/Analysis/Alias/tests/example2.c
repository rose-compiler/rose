int main()
{
   // Must alias
   int *p, *q;
   int i, j;
   i = 10; j = 5;
   p = &i;    *p++;
   p = &j;    *p--;
   q = p;  *q--;
   return 0;
}
