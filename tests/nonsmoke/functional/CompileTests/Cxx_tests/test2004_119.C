
template <typename T> class X;
template <typename T> class Y;

typedef X<int> X_int;
typedef Y<X_int> Y_int;

X_int *object_X;
Y_int *object_Y;

