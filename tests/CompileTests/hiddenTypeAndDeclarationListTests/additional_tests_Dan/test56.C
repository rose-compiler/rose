// test for templates

template <class T>
T min(T a, T b)
{
    return (a < b) ? a : b;
}

void foo(int r) {}


template<class T> class Stack {

	public:

		Stack() : maxStack(256) { s = new T[maxStack]; index=0; }
		~Stack() { delete[] s; }

		bool pop(T *);

		bool push(T );

		bool push(T*, T*);

		// bool push(int in); // not allowed

		bool push(int i, double d);

		bool min(T a, T b, T c); // hides the template function min!!

	private:

    		const int maxStack;
		T *s;
		int index;

};


int main() {

	int i = 15;
        int j = 16;

        int x  = min(i,j);

	{

		// int min, wwwwwwwwwwwwwww;

		{

			/*double min;

			double a, b;

			double f = min(a, b);*/

		}

	}

        foo(i);

	Stack<int> iStack;
	int v = 2;
	int* t = &v;
//	iStack.pop(t);

  	return 0;

}
