#define INDEFINITION

template < typename T > class ISpace
{
	public:
	class theiterator
	{
		protected:

//		int _max;

		public:

		theiterator( )
#ifdef INDEFINITION
		{
// 		_max = 10;
		}
#else
		;
#endif
	};

#if 1
theiterator begin() const
	{
		return theiterator( );
	}
#endif
};

#ifndef INDEFINITION
template < typename T > ISpace<T>::theiterator::theiterator( )
{
	_max = 10;
}
#endif

void foo_main()
{
//	printf("Hello world\n");

	ISpace<int> space;

	ISpace<int>::theiterator i = space.begin();
//	ISpace<int>::theiterator i;

//	return 0;
}
