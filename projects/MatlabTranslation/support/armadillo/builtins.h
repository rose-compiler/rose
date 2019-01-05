#ifndef FASTNUM_BUILTINS
#define FASTNUM_BUILTINS

#include <iostream>
#include <sstream>
#include <cmath>
#include <ctime> // \todo replace with chrono
#include <tuple>
#include <cassert>
#include <armadillo>

#include "Matrix.h"

namespace fastnumbuiltin
{
  template <class T>
  static inline
  double det(const Matrix<T>& m)
  {
    return ::arma::det(m);
  }

  static inline
  Matrix<double> randn(int m, int n)
  {
    return Matrix<double>(arma::randn(m, n));
  }

  static inline
  Matrix<double> randn(int m)
  {
    return ::arma::randn(m,m);
  }

  static inline
  double randn()
  {
    Matrix<double> m = ::arma::randn(1);

    return m.at(0);
  }


  //~ //uniform distribution
  static inline
  auto rand(int m, int n) -> decltype(arma::randu(m, n))
  {
    return arma::randu(m, n);
  }

  static inline
  auto rand(int m) -> decltype(rand(m,m))
  {
    return rand(m,m);
  }

  static inline
  double rand0()
  {
    Matrix<double> m = rand(1);

    return m.at(0);
  }

  template <class T>
  static inline
  void disp(T item)
  {
    std::cout << item << std::endl;
  }

  template <class ...T, class E>
  static inline
  void disp(E el, T... args)
  {
    disp(el);
    disp(args...);
  }

  template <class T>
  static inline
  T power(T num, int power)
  {
    return static_cast<T>( std::pow(num, power) );
  }

  template <class T>
  static inline
  Matrix<T> power(Matrix<T> m, int n)
  {
    assert(false);
    return m;
  }

  //~ template <class T>
  //~ static inline
  //~ std::tuple<int, int> size(const Matrix<T>& m)
  //~ {
    //~ arma::Mat<T>& matrix = m.getInnerMatrix();
//~
    //~ return std::make_tuple<int, int>(matrix.n_rows, matrix.n_cols);
  //~ }

  template <class T>
  static inline
  Matrix<T> sum(const Matrix<T>& m)
  {
    return Matrix<T>(arma::sum(m));
  }

  static inline
  Matrix<double> zeros(int r, int c)
  {
    return Matrix<double>(arma::zeros(r, c));
  }

  template <class T>
  static inline
  T norm(const Matrix<T>& m)
  {
    return norm(m);
  }

  template <class T>
  static inline
  auto transpose(const Matrix<T>& m) -> decltype(m.t())
  {
    return m.t();
  }

  template <class T>
  static inline
  auto transpose(Matrix<T>&& m) -> decltype(m.t())
  {
    return m.t();
  }

  template <class T>
  static inline
  Vec<T>
  max(const Matrix<T>& m)
  {
    return ::arma::max(m);
  }

  template <class T>
  static inline
  auto diag(Matrix<T>&& m) -> decltype(m.diag())
  {
    return m.diag();
  }

  template <class T>
  static inline
  auto diag(Matrix<T>& m) -> decltype(m.diag())
  {
    return m.diag();
  }

  template <class T>
  static inline
  auto diag(Vector<T>&& v) -> decltype(::arma::diagmat(v))
  {
    return ::arma::diagmat(v);
  }

  // \todo use template magic to extract the type from arma::eOp
  template <class T1, class T2>
  static inline
  auto diag(typename ::arma::eOp<T1, T2> mop) -> Vector<double>
  {
    Matrix<double> m(std::move(mop));

    return m.diag();
  }

  template <class T>
  static inline
  auto diag(typename ::arma::diagview<T> v) -> Matrix<T>
  {
    return ::arma::diagmat(v);
  }


  template <class U, class V>
  static inline
  auto
  mldivide(const U& m1, const V& m2) -> decltype(::arma::solve(m1, m2))
  {
    return ::arma::solve(m1, m2);
  }

  template <class U, class V>
  static inline
  auto
  mrdivide(const U& m1, const V& m2) -> decltype(::arma::solve(m2.t(), m1.t()).t())
  {
    return ::arma::solve(m2.t(), m1.t()).t();
  }

  template <class T>
  static inline
  auto
  rdivide(const Matrix<T>& m1, const Matrix<T>& m2) -> decltype(m1 / m2)
  {
    return m1 / m2;
  }

  template <class U, class V>
  static inline
  auto
  times(const U& m1, const V& m2) -> decltype(m1 * m2)
  {
    return m1 * m2;
  }

/*
 * we use m1 * m2

  template <class T>
  static inline
  auto
  mtimes(const Matrix<T>& m1, const Matrix<T>& m2) -> decltype(m1 / m2)
  {
    return m1 % m2;
  }
*/

  template <class T>
  static inline
  std::tuple<int, int> sizeM(const Matrix<T>& mat)
  {
    ::arma::SizeMat sz = ::arma::size(mat);

    return std::tuple<int,int>(sz.n_rows, sz.n_cols);
  }

  // using std::sqrt;
  // using arma::sqrt;


  //
  // support for conversions to strings
  template <class T>
  static inline
  std::string num2str(const T& val)
  {
    std::stringstream buf;

    buf << val;
    return buf.str();
  }

  //
  // tic toc support
  // \move to cc file

  thread_local double tictoc_timepoint;

  static inline
  double currentTime(void)
  {
    timespec   tv;
    const bool fail = clock_gettime(CLOCK_REALTIME, &tv);

    assert(!fail);
    return tv.tv_sec + (tv.tv_nsec / 1000000000.0);
  }

  static inline
  double tic()
  {
    return tictoc_timepoint = currentTime();
  }

  static inline
  double toc(double starttime)
  {
    return currentTime() - starttime;
  }

  static inline
  double toc()
  {
    return toc(tictoc_timepoint);
  }
}

#endif /* FASTNUM_BUILTINS */
