template <typename T> class B{};

typedef int C;
B<C> x;
B< B<C> > y;


