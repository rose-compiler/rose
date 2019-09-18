 template<typename T>
 void swap(T& x, T& y)
 {
   T tmp = x;
   x = y;
   y = tmp;
 }

int foo (int a, int b)
{
   swap(a,b);
}

int main()
{
}
