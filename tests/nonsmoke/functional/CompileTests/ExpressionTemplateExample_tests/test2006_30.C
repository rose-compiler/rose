#include<iostream>
#include<fstream>
using namespace std;

template<class A> struct X{ };

template <class L, class R>
class Add : public X<Add<L,R> >{
 private:
  const L& l_;
  const R& r_;
 public: 
  Add(const L& l, const R& r) : l_(l), r_(r){}
  double gv(int i) const {return l_.gv(i)+r_.gv(i);}
};

template <class L, class R>
class Mult : public X<Mult<L,R> >{
 private:
  const L& l_;
  const R& r_;
 public: 
  Mult(const L& l, const R& r) : l_(l), r_(r){}
  double gv(int i) const {return l_.gv(i)*r_.gv(i);}
};

class Vec : public X<Vec>{
 private: 
  double* dt;
  const int n_;
 public:
  Vec(int n, double wert) : n_(n) {
    dt = new double[n_];
    for (int i=0;i<n_;++i)
       dt[i] = wert;
  }
  ~Vec() {delete[] dt;}
  template<class A>
  void operator=(const X<A>& x){
    for(int i= 0; i < n_ ; ++i) 
      dt[i] = static_cast<const A&>(x).gv(i);
  }

  double gv(int i) const { return dt[i];}
};

template <class L, class R>
inline Add<L,R> operator+(const X<L>& l, const X<R>& r){
  return Add<L,R>(static_cast<const L&>(l), static_cast<const R&>(r));
}

template <class L, class R>
inline Mult<L,R> operator*(const X<L>& l, const X<R>& r){
  return Mult<L,R>(static_cast<const L&>(l), static_cast<const R&>(r));
}
int main() {
  std::cout.precision(3);
  ifstream PARAMETER;
  int i, iter, N = 10;

#if 0
  PARAMETER.open("para.dat",ios :: in);
  PARAMETER >> N >> iter;
  PARAMETER.close();
#endif

  Vec A_(N,0.);
  Vec B_(N,3.);
  Vec C_(N,5.);
  Vec D_(N,7.);
  Vec E_(N,9.);

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
#endif
       );
  i++;
  if(i < iter)
    goto marke; 

    return 0;
}

