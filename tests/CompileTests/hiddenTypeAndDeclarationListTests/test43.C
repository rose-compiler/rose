// test for template classes

template<class T> class Stack {

	public:

		Stack() : maxStack(256) { s = new T[maxStack]; index=0; }
		~Stack() { delete[] s; }

		bool pop(T *);

		bool push(T );

		bool push(T*, T*);

		// bool push(int in); // not allowed

		bool push(int i, double d);

	private:

    		const int maxStack;
		T *s;
		int index;

};

/*
template<class T> bool Stack<T>::pop(T *get)
{

	int kkkkkkkkkk;

	if (index==0 ) return false;
	*get = s[--index];
	return true;

}
*/

// DQ (9/12/2009): error with GNU g++ version 4.3 and greater
// error: "int main(int)" takes only zero or two arguments
// and the types are enforced by GNU g++ as well.
// int main (int tttttttttt)
int main ()
{
	Stack<int> iStack;
	int v = 2;
	int* t = &v;
//	iStack.pop(t);

	Stack<double> dStack;

	{

		int qqqqqqqqqqqqqqqqqqqq;
		float Stack;

	}

	/*Stack<float> fStack;

	Stack<unsigned int> uiStack;*/

	return 0;

}
