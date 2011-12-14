
#ifndef MATRIX_TMPL_H
#define MATRIX_TMPL_H

#include <assert.h>
#include <unistd.h>

template <class Mat, class T>
class SubMatrix {
     unsigned r,c;
     Mat& mat;
   public:
     SubMatrix(Mat& m, unsigned _r, unsigned _c) : mat(m), r(_r),c(_c) {}
     unsigned cols() const { return mat.cols()-c; }
     unsigned rows() const { return mat.rows()-r; }
     T& Entry(unsigned index1, unsigned index2) const
      { return mat.Entry(index1+r, index2+c); }
     T& Entry(unsigned index1, unsigned index2) 
      { return mat.Entry(index1+r, index2+c); }
  };

template <class Mat, class T>
class RowVector {
     unsigned r,c,len;
     Mat& mat;
   public:
     RowVector(Mat& m, unsigned _r, unsigned _c, unsigned _len) 
          : mat(m), r(_r),c(_c), len(_len) { assert(len >= c && len < mat.cols()); }
     unsigned size() const { return len-c; }
     void push_back( const T& e) 
       { mat.Entry(r,len) = e; ++len; }
     T& Entry(unsigned index) const
      { assert(index + c < len); return mat.Entry(r, index+c); }
     T& operator [](unsigned index) const
      { return Entry(index); }
  };


template <class T>
class Matrix 
{
  unsigned nr, nc, num;
  T* vec;
  unsigned GetLength() const { return num; }
 public:
  typedef T Elem;

  Matrix() { nr = nc = num = 0; vec = 0; }
  Matrix(unsigned _nr, unsigned _nc, unsigned spare = 1)
  { nr = _nr; nc = _nc;
    num = (nr+spare) * (nc+spare);
    if (num == 0)
       vec = 0;
    else
       vec = new T[num];
  }

  Matrix(const Matrix &that)
  { nr = that.nr; nc = that.nc; num = that.num;
    if (num == 0) 
       vec = 0;
    else {
       vec = new T[num];
       for (size_t i = 0; i < num; ++i)
          vec[i] = that.vec[i];
    }
  }
  void operator = (const Matrix &that)
  { nr = that.nr; nc = that.nc;
    if (num < that.num) {
      delete [] vec;
      num = that.num;
      vec = new T[num];
    }
    if (that.num > 0) {
       for (int i = 0; i < num; ++i)
          vec[i] = that.vec[i];
    }
  }

  ~Matrix()
  { if (vec != 0)
      delete [] vec;
  }

  unsigned cols() const { return nc; }
  unsigned rows() const { return nr; }
  T& Entry(unsigned index1, unsigned index2) const
   { unsigned index = index1 * nc + index2;
     return vec[index];
   }

  T& operator()(unsigned index1, unsigned index2) const
   {  return Entry(index1, index2); }

  RowVector<Matrix<T>,T> operator[](int index) 
      { 
     // DQ (11/8/2011): Fixed to avoid function pointer comparision ("rows()" instead of "rows"). 
        assert(index > 0 && index < rows()); 
        return RowVector<Matrix<T>, T>(*this, index, 0, cols());
      }

  RowVector<const Matrix<T>,T> operator[](int index) const 
      {
     // DQ (11/8/2011): Fixed to avoid function pointer comparision ("rows()" instead of "rows"). 
        assert(index > 0 && index < rows()); 
        return RowVector<const Matrix<T>, T>(*this, index, 0, cols());
      }

  void Reset(unsigned index1, unsigned index2)
  {
    unsigned num1 = index1 * index2;
    nr = index1; nc = index2;
    if (num < num1) {
      delete [] vec;
      num = num1;
      vec = new T[num];
    }
  }

  void Initialize(const T &init)
   { for (size_t i = 0; i < num; i++)
       vec[i] = init;
   }

  bool IsEmpty() const { return vec == 0; }


};

template<class Mat, class ElemOp>
bool UpdateMatrix(Mat& d1, const Mat &d2, ElemOp op)
{
  assert (d1.rows() == d2.rows() && d1.cols() == d2.cols());
  bool mod = false;
  for (int i = 0; i < d1.rows(); i++) {
    for (int j = 0; j < d1.cols(); j++) {
      typename Mat::Elem e1 = d1.Entry(i,j);
      if ( op(e1, d2.Entry(i,j))) {
        mod = true;
        d1.Entry(i,j) = e1;
      }
    }
  }
  return mod;
}


#endif

