template <class C>
class scoped_ptr 
   {
     public:
//        typedef C element_type;

          C& operator*() const;
//        C* ptr_;
   };

template <class C>
class scoped_array 
   {
     public:
          C& operator[](int i) const;
   };

class Y {};

void foo()
   {
     scoped_array<scoped_ptr<Y> > X;

  // scoped_ptr<Y> a = X[42];

  // Y b = *X[7];
     *X[7];

  // Y c = *a;
   }
