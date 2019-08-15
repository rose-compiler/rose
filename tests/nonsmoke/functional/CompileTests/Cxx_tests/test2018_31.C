class W;
template<class T> struct X {
	struct Y { };
        int f(const W&);
	struct Z {
		int g() { return 8; }
	};
};

#if 1
class W {
     // template<class T> friend struct X<T>::Y;
        template<class T> friend int X<T>::f(const W&);
        int member;
public:
        W (int i) : member(i) { }
};
#endif

