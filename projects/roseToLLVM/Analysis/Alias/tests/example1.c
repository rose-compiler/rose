int main()
{
   // Must alias
   int *p, i, j;
   i = 10; j = 5;
   p = &i;
   *p++;
   p = &j;
   *p--;
   i--;
   i = i + 5;
   return 0;
}
