
template <class T>
struct x_
   {
     T f(T i) { return i; }
     T f(T i) const { return i + 10; }
   };

template <class T>
struct y_ : public x_<T>
   {
     T f(double i) { return (T)i + 100; }
   };


void foobar()
   {
     y_<int> o;
     (o.f)(1);
     const y_<int>* pcy = &o;

  // Original code: ((pcy->x_<int>::f))(2);
  // Unparsed as:     pcy -> f(2);
     ((pcy->x_<int>::f))(2);

     (o.x_<int>::f)(1);

  // Original code: ((pcy->x_<int>::f))(2);
  // Unparsed as:     pcy -> f(2);
     ((pcy->x_<int>::f))(2);
   }

