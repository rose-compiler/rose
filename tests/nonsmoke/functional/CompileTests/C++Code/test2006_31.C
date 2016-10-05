#include<iostream>
#include<fstream>
using namespace std;

template<class A> struct Expr{};

template <class A, class B>
struct Add : public Expr<Add<A,B> >{
  static inline double Give(int i) {return (A::Give(i)+B::Give(i));}
};

template <class A, class B>
inline Add<A,B> operator+(const Expr<A>& a, const Expr<B>& b){return Add<A,B>();}

template <class A, class B>
struct Mult : public Expr<Mult<A,B> >{
  static inline double Give(int i) {return (A::Give(i)*B::Give(i)); }
};

template <class A, class B>
inline Mult<A,B> operator*(const Expr<A>& a, const Expr<B>& b){return Mult<A,B>();}

template<int num>
class Vector : public Expr<Vector<num> >{
 private: 
  int n_;
  static double* data_;
 public:
  Vector(int n, double wert) : n_(n){ 
    data_ = new double[n_];
    for(int i=0; i < n_; ++i)
      data_[i] = wert; 
  }
  ~Vector(){delete[] data_;}
 
  template<class A>
  inline void operator=(const Expr<A>& a) { 
    int N = n_, i=0;
    double* dd = data_;
    for(i = 0; i < N; ++i)
       dd[i] = A::Give(i);
  }
  static inline double Give(int i) {return data_[i];}
  static inline double* data() {return data_;}

};
template <int num> double *  Vector<num>::data_;

int main() {
  std::cout.precision(3);
  ifstream PARAMETER;
  int i, iter, N = 10;

#if 0
  PARAMETER.open("para.dat",ios :: in);
  PARAMETER >> N >> iter;
  PARAMETER.close();
#endif
  
  Vector<1> A_(N,0.);
  Vector<2> B_(N,3.);
  Vector<3> C_(N,5.);
  Vector<4> D_(N,7.);
  Vector<5> E_(N,9.);

  i = 0;
  marke:
    A_ = (
// #include "expression.code"
  A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ 

+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
#if 0
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
#endif
         );
  i++;
  if(i < iter)
    goto marke; 
  return 0;
}
