// Test code from Imperial:
// Paul Kelly told me to e-mail you with any comments and bugs for Rose.
// Rose as a tool looks perfect for what we're trying to do, extracting kernels 
// with indexed metadata from C++ code. In doing that I have a set of iterators, 
// and as a result I've been experimenting with the template support, specifically 
// for a class in a templated class. Here's a little example that shows an apparent 
// bug in rose that your people are far more likely to be able to fix than I am. If 
// INDEFINITION is defined in the example below, then there is a link error because 
// rose appears to lose the definition of the functions in the inner class. If 
// INDEFINITION is not defined, then the explicit declarations are used, and the 
// output is perfectly sound. Obviously this is a bug (assuming it is one) that can 
// be worked round easily, but it might be useful to put it into a test suite anyway. 
// I would have put this in the bug reporting system, but it doesn't appear to be very 
// active so I wasn't sure if you were really using it.

// This is all on the current release version of rose, 0.9.3a.

// Many thanks, and thanks again for a useful tool,
// Lee Howes

#include <stdio.h>

#define INDEFINITION

template < typename T > class ISpace
{
	public:
	class theiterator
	{
		protected:

		int _max;

		public:

		theiterator( )
#ifdef INDEFINITION
		{
			_max = 10;
		}
#else
		;
#endif
	};

theiterator begin() const
	{
		return theiterator( );
	}
};

#ifndef INDEFINITION
template < typename T > ISpace<T>::theiterator::theiterator( )
{
	_max = 10;
}
#endif

int main()
{
	printf("Hello world\n");

	ISpace<int> space;

	ISpace<int>::theiterator i = space.begin();

	return 0;
}
