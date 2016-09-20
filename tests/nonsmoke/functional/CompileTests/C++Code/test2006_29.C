// Jochen's Classical Expression Template Example

#include<iostream>
#include<fstream>
using namespace std;

struct Times {
public:
  inline static double apply(double a, double b) {
    return a*b;
  }
};

struct Add {
public:
  inline static double apply(double a, double b) {
    return a+b;
  }
};

template<typename Left, typename Op, typename Right>
struct X {
  const Left &leftNode_;
  const Right &rightNode_;

  X(const Left &t1, const Right &t2) : leftNode_(t1), rightNode_(t2) {}

  inline double Give(int i) const {
    return Op::apply(leftNode_.Give(i), rightNode_.Give(i));
  }
};

template<typename Right>
struct X<double,Times,Right> {
  const double& leftNode_;
  const Right &rightNode_;

  X(const double &t1, const Right &t2) : leftNode_(t1), rightNode_(t2) {}

  inline double Give(int i) const {
    return Times::apply(leftNode_, rightNode_.Give(i));
  }
};

class Vector {
 private:
// double*restrict data_;
  double* data_;
  int N_;
 public:
  Vector(int N, double wert) : N_(N) {
    int i= 0;
    data_ = new double[N_];
    for(i=0; i < N_; i++)
      data_[i] = wert;
  }
  ~Vector() {delete[] data_;}
  template<typename Left, typename Op, typename Right>
  inline void operator=(const X<Left, Op, Right> &expression) {
    int N = N_, i = 0;
 // double*restrict dd = data_;
    double* dd = data_;
    for(i=0; i < N; ++i)
      dd[i] = expression.Give(i);
  }

  inline double Give(int i) const {
    return data()[i];
  }
// inline double*restrict data() const {return data_;}
  inline double* data() const {return data_;}
};

template<typename Left>
X<Left, Times, Vector> operator*(const Left &a, const Vector &b) {
  return X<Left, Times, Vector >(a, b);
}

template<typename Left, typename Right>
X<Left, Times, Right> operator*(const Left &a, const Right &b) {
  return X<Left, Times, Right>(a, b);
}

template<typename Left>
X<Left, Add, Vector> operator+(const Left &a, const Vector &b) {
  return X<Left, Add, Vector >(a, b);
}

template<typename Left, typename Right>
X<Left, Add, Right> operator+(const Left &a, const Right &b) {
  return X<Left, Add, Right>(a, b);
}

int main() {
  ifstream PARAMETER;
  int i, iter, N = 10;

#if 0
  PARAMETER.open("para.dat",ios :: in);
  PARAMETER >> N >> iter;
  PARAMETER.close();
#endif

  Vector A_(N,0.);
  Vector B_(N,3.);
  Vector C_(N,5.);
  Vector D_(N,7.);
  Vector E_(N,9.);

  i = 0;
  marke:
    A_ = (
// 64 operator+ calls works, but 65 triggers and interna error
//
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
#endif
         );
  i++;
  if(i < iter)
    goto marke; 
  return 0;
}
