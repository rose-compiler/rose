void* foo(int x);
#       define tnalloc(T,n) ((T*) foo(sizeof(T)*(n)))

int main(int argc, char** argv)
{
  int chars_max;
        tnalloc(char, chars_max);
        tnalloc(char const*, 1 + argc + !!0 + 2*1 + 2);

};
