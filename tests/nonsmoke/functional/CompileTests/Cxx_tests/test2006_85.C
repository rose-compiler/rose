/*
Hi Dan,

I found some problems in the unparser regarding the declaration of a
variable with a constructor initializer within the condition of an if,
while or for statement.  The problem occurs because C++ requires any
such variable to use the pseudo-assignment '=' syntax, which is not the
syntax currently used by the unparser.  The attached patches fix this bug
along with a minor bug in the output of while statements which declare
such a variable.  Also attached is a test case.

Thanks,
-- Peter
*/

struct A
   {
     int i;

     A()
	{
	}

     A(int i) : i(i)
        {
        }

     ~A()
	{
	}

     operator bool()
	{
	  return false;
	}
   };

int main()
   {
     while (A a = 2)
	{
	}
     if (A a = 2)
	{
	}
     for (;A a = 2;)
	{
	}
   }
