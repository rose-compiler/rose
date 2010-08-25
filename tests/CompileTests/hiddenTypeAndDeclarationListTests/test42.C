// test for template functions

template <class T>
T min(T a, T b)
{
    return (a < b) ? a : b;
}

void foo(int r) {}

int main() {

	int i = 15;
        int j = 16;

        int x  = min(i,j);

	{
  // "int min" can hide the template function "min" and the template instantiation "min"
                int min, wwwwwwwwwwwwwww;

		{

			/*double min;

			double a, b;

			double f = min(a, b);*/

		}

	}

        foo(i);

  	return 0;

}


