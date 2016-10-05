// void foo (struct X *ptr);

// enum values; forward enum declarations are not allowed in C or C++ (and not supported in ROSE, though they work in EDG)
// void foobar ( values v);
// enum values { a,b,c };

// Here the defining and nodefining declarations should be the same
// void foo() {};

void foobar();

void foobar();

void foobar() {}

#if 1
void foobar();

template <typename T>
void foobar(T t);

template <typename T>
void foobar(T t) {}

template <typename T>
void foobar(T t);

template <> void foobar<int> (int t);

template <> void foobar<int> (int t) {};

template <> void foobar<int> (int t);
#endif
