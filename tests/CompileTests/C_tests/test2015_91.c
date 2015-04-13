/* Test code used to show that "long long" is the same as "long long int". */

typedef unsigned long int testtype;

extern unsigned int foobar(unsigned long long int __dev);

int main()
   {
     testtype x = 1;
     return 0;
   }
