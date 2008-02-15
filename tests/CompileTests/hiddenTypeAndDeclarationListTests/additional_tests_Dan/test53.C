// test for template functions, 2

template <class T>
T min(T a, T b)
{
    return (a < b) ? a : b;
}

template <class T>
T min(T a, T b, T c)
{
    return (a < c) ? a : c;
}

void foo(int r) {}

int main() {

	int i = 15;
        int j = 16;

        int x  = min(i,j);

	{

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
