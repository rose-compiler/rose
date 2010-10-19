template <class T>
class templated
{
	T val;
	public:
	templated();
	
	T getVal();
};

template <class T>
templated<T>::templated(){}

template <class T>
T templated<T>::getVal() { return val; }
