/*
 * Matrix.h
 *
 */

#ifndef _MATRIX_H
#define _MATRIX_H

#include <cassert>
#include <iostream>
#include <armadillo>


struct MatlabSpan
{
  int start;
  int end;
};

enum MatlabSymbol
{
  COLON,
  endr
};

namespace fastnumbuiltin
{
  template <class T>
  using Matrix = ::arma::Mat<T>;

  template <class T>
  using Vector = ::arma::Col<T>;

  template <class T>
  using Vec = ::arma::Row<T>;

  template<typename T>
  struct Range
  {
      Range(T _start, T _stride, T _end)
      : start(_start), stride(_stride), end(_end)
      {}
              
      Range(T _start, T _end)
      : Range(_start, 1, _end)
      {}
  
      Range()
      : Range(0, 0, 0)
      {}
      
      void setBounds(T _start, T _end)
      {
        start = _start;
        stride = 1;
        end = _end;
      }
    
      void setBounds(T _start, T _stride, T _end)
      {
        start = _start;
        stride = _stride;
        end = _end;
      }
    
      /**
       * getMatrix is called on a range expression
       *   eg. funcCall(1:2:3) will be funcCall(range.getMatrix()) after creating range object
       *   or x = 1:2:3 will be x = range.getMatrix() after creating range object
       *   or A = [1, 2, 3; 4  5 6]
       *   A(1:2, :) will be A(range.getMatrix(), :)
       */
      Matrix<T> getMatrix()
      {
        int numOfElements = (end - start) / stride + 1;
    
        if (numOfElements <= 0) return Matrix<T>();
            
        Matrix<T> matrix(1, numOfElements);
        T         start_element = start;
    
        for (size_t i = 0; i < numOfElements; ++i)
        {
          matrix(i) = start_element;
    
          start_element += stride;
        }
    
        return matrix;
      }

    private:
      T start;
      T stride;
      T end;
  };
}


#if UAB_MATRIX

template<typename T>
class Matrix;

template<class T>
Matrix<double> operator*(double factor, const Matrix<T>& m);

/*
 * Iterator for the Matrix<T> class
 * Can be used in a for loop as
 * for(Matrix<int>::iterator i = m.begin(); i != m.end(); ++i)
*/
template<typename ITERATOR_TYPE, typename ELEMENT_TYPE>
class MatrixIterator
{
 private:
  ITERATOR_TYPE it;
 public:
  MatrixIterator(ITERATOR_TYPE it)
    {
      this->it = it;
    }

  ELEMENT_TYPE operator*()
    {
      return *it;
    }

  //prefix
  MatrixIterator<ITERATOR_TYPE, ELEMENT_TYPE>& operator++()
    {
      ++it;

      return *this;
    }

  //postfix
  MatrixIterator<ITERATOR_TYPE, ELEMENT_TYPE>& operator++(int)
    {
      it++;

      return *this;
    }

  bool operator!=(const MatrixIterator<ITERATOR_TYPE, ELEMENT_TYPE> &other)
  {
    return this->it != other.it;
  }
};


template<class T>
  class Matrix
  {
  public:
    Matrix () 
    : matrix ()
    {}

    Matrix(std::initializer_list<T> list) : matrix(list)
    {
    }

    Matrix (int row, int col)
    : matrix (row, col)
    {}

    explicit
    Matrix (arma::Mat<T> mat)
    : matrix(mat)
    {}

    int get_row ();
    int get_col ();

    template<typename C>
    Matrix<std::complex<C>>
    fft();

    template<typename C>
    Matrix<std::complex<C>>
    ifft();

    Matrix
    submat (int row_start, int row_end, int col_start, int col_end);

    Matrix<T>&
    operator=(const Matrix<T>& other)
    {
      if (this != &other)
      {
        this->matrix = other.matrix;
      }

      return *this;
    }

    Matrix<T>&
    operator=(const T scalar)
    {
      this->matrix = scalar;
      return *this;
    }

    //>>>>>>> constraintgen
    Matrix<T> operator() (std::initializer_list<arma::uword> rowList, std::initializer_list<arma::uword> colList)
    {
      arma::uvec rowVector = rowList;
      arma::uvec colVector = colList;

      Matrix<T> submatrix(matrix.submat(rowVector, colVector));

      return submatrix;
    }

    T& operator() (const unsigned int row, const unsigned int col)
    {
      return matrix(row - 1, col - 1); //Armadillo's index start with 0 whereas Matlab's with 1
    }

    //linear access column major order
    T& operator() (const unsigned int index)
    {
      return matrix(index - 1);
    }

    //individual element access A([1 2 3])
    Matrix<T> operator() (std::initializer_list<arma::uword> indicesList)
    {
      arma::uvec indices_vec(indicesList.size());

      int i = 0;
      for(arma::uword index : indicesList)
      {
        indices_vec[i++] = index - 1;
      }

      Matrix<T> elementMatrix(matrix.elem(indices_vec).st());
      return elementMatrix;
    }

    //A(:, 2)
    Matrix<T> operator() (MatlabSymbol symbol, int col)
    {
      assert(symbol == MatlabSymbol::COLON);

      Matrix<T> column(matrix.col(col - 1));

      return column;
    }

    //A(2, :)
    Matrix<T> operator() (int row, MatlabSymbol symbol)
    {
      assert(symbol == MatlabSymbol::COLON);

      Matrix<T> rowMatrix(matrix.row(row - 1));

      return rowMatrix;
    }

    arma::uvec toUvec(Matrix<int> rowList)
    {
      arma::Mat<int> internalMatrix = rowList.getInternalMatrix();
      int numElem = internalMatrix.n_cols;

      arma::uvec rowIndices(numElem);

      for(int i = 0; i < numElem; i++)
      {
        int element = internalMatrix.at(0, i) - 1;

        rowIndices.at(i) = element;
      }

      // std::cout << rowIndices << std::endl;
      return rowIndices;
    }

    //A([1:2:3], :)
    Matrix<T> operator() (Matrix<int> rowList, MatlabSymbol symbol)
    {
      assert(symbol == MatlabSymbol::COLON);


      arma::uvec rowIndices = toUvec(rowList);


      Matrix<T> subRows(matrix.rows(rowIndices));
      return subRows;
    }

    //A(:, [1:2:3])
    Matrix<T> operator() (MatlabSymbol symbol, Matrix<int> colList)
    {
      assert(symbol == MatlabSymbol::COLON);

      arma::uvec colIndices = toUvec(colList);

      Matrix<T> subRows(matrix.cols(colIndices));
      return subRows;
    }

    //A([1:2:3], [1:2:3])
    Matrix<T> operator() (Matrix<int> rowList, Matrix<int> colList)
    {
      arma::uvec rowIndices = toUvec(rowList);
      arma::uvec colIndices = toUvec(colList);

      Matrix<T> subRows(matrix(rowIndices, colIndices));
      return subRows;
    }

    arma::Mat<T> getInternalMatrix()
    {
      return this->matrix;
    }

    //A(1:3, :)
    Matrix<T> operator() (MatlabSpan rowSpan, MatlabSymbol symbol)
    {
      assert(symbol == MatlabSymbol::COLON);

      Matrix<T> rows(matrix.rows(rowSpan.start - 1, rowSpan.end - 1));

      return rows;
    }

    //A(:, 1:3)
    Matrix<T> operator() (MatlabSymbol symbol, MatlabSpan colSpan)
    {
      assert(symbol == MatlabSymbol::COLON);

      Matrix<T> cols(matrix.cols(colSpan.start - 1, colSpan.end - 1));

      return cols;
    }

    //A(1:3, n)
    Matrix<T> operator() (MatlabSpan rowSpan, int col)
    {
      Matrix<T> rows(matrix(arma::span(rowSpan.start - 1, rowSpan.end - 1), col - 1));

      return rows;
    }

    //A(n, 1:3)
    Matrix<T> operator() (int row, MatlabSpan colSpan)
    {
      Matrix<T> cols(matrix(row - 1, arma::span(colSpan.start - 1, colSpan.end - 1)));

      return cols;
    }


    //ITERATORS

    typedef MatrixIterator<typename arma::Mat<T>::iterator,T> iterator;

    iterator begin()
      {
        iterator it(this->matrix.begin());

        return it;
      }

    iterator end()
      {
        iterator it(this->matrix.end());

        return it;
      }

    //ITERATORS:END

    Matrix<T>& operator+ (const Matrix<T>& other)
    {
      Matrix<T> *m = new Matrix(this->matrix + other.matrix);
      return *m;
    }

    Matrix<T>& operator- (const Matrix<T>& other)
    {
      Matrix<T> *m = new Matrix(this->matrix - other.matrix);
      return *m;
    }

    Matrix<T>& operator* (const Matrix<T>& other)
    {
      Matrix<T> *m = new Matrix(this->matrix * other.matrix);
      return *m;
    }

    Matrix<T>& operator/ (const T val)
    {
      Matrix<T> *m = new Matrix(this->matrix / val);

      return *m;
    }

    Matrix<T>& operator/ (const Matrix<T>& scalarMatrix)
    {
      Matrix<T> *m = new Matrix(this->matrix / scalarMatrix.matrix(0));
      // arma::as_scalar(scalarMatrix.matrix));

      return *m;
    }

    friend
    std::ostream& operator<< (std::ostream& os, const Matrix<T> m)
    {
      os << m.matrix;
      return os;
    }

    arma::mat_injector< arma::Mat<T> > operator<<(const T val)
    {
      return matrix << val;
    }

    arma::mat_injector< arma::Mat<T> > operator<<(const arma::injector_end_of_row<>& x)
    {
      return matrix << x;
    }

    arma::mat_injector< arma::Mat<T> > operator<<(const MatlabSymbol endRow)
    {
      return matrix << arma::endr;
    }

    Matrix<T>
    add (T val)
    {
      matrix << val;
      return *this;
    }

    void
    endr ()
    {
      matrix << arma::endr;
    }

    /* Vector<T> */
    /* getVector () */
    /* { */
    /*   std::vector<T> vec (matrix.begin (), matrix.end ()); */
    /*   arma::Row<T> v (vec); */
    /*   Vector<T> vector (v); */
    /*   return vector; */
    /* } */

    /* Matrix<T> */
    /* transpose () */
    /* { */
    /*   arma::Mat<T> t_matrix = matrix.t (); */
    /*   Matrix<T> mat (t_matrix); */
    /*   return mat; */
    /* } */

    /* Vector<T> */
    /* getRow (int r) */
    /* { */
    /*   Vector<T> result (matrix.row (r)); */
    /*   return result; */
    /* } */

    /* Vector<T> */
    /* getCol (int c) */
    /* { */
    /*   Vector<T> result (matrix.col (c).t ()); */
    /*   return result; */
    /* } */

    void
    print (std::string header)
    {
      matrix.print (header);
    }

    /* static Matrix<T> */
    /* cos (Vector<T> v) */
    /* { */
    /*   arma::Mat<T> cosOfVector = arma::cos (v.get ()); */
    /*   Matrix<T> cosMatrix (cosOfVector); */
    /*   return cosMatrix; */
    /* } */

    /* static Matrix<T> */
    /* sin (Vector<T> v) */
    /* { */
    /*   arma::Mat<T> sinOfVector = arma::sin (v.get ()); */
    /*   Matrix<T> sinMatrix (sinOfVector); */
    /*   return sinMatrix; */
    /* } */

    /* static Vector<T> */
    /* mean (Matrix<T> m) */
    /* { */
    /*   Vector<T> vector (arma::mean (m.matrix)); */
    /*   return vector; */
    /* } */

    /* static Vector<T> */
    /* median (Matrix<T> m) */
    /* { */
    /*   Vector<T> vector (arma::median (m.matrix)); */
    /*   return vector; */
    /* } */

    static Matrix<double> randn(int m, int n)
    {
      arma::mat random_matrix = arma::randn(m, n);

      Matrix<T> result(random_matrix);

      return result;
    }

    const arma::Mat<T>& getInnerMatrix() const { return matrix; }
    const arma::Mat<T>& inner() const { return matrix; }

    double det()
    {
      return arma::det(this->matrix);
    }

    Matrix<T> t() const
      {
        return Matrix<T>(this->matrix.t());
      }

    Matrix<double> friend ::operator* <> (double factor, const Matrix<T>& m);

  private:
    arma::Mat<T> matrix;
  };

template<class T>
  int
  Matrix<T>::get_col ()
  {
    return matrix.n_cols;
  }

template<class T>
  int
  Matrix<T>::get_row ()
  {
    return matrix.n_rows;
  }

template<class T>
  template<typename C>
    Matrix<std::complex<C>>
    Matrix<T>::fft ()
    {
      arma::Mat<std::complex<C>> fftOfM = arma::fft2 (matrix);
      Matrix<std::complex<C>> mat (fftOfM);
      return mat;
    }

template<class T>
  template<typename C>
    Matrix<std::complex<C>>
    Matrix<T>::ifft ()
    {
      arma::Mat<std::complex<C>> fftOfM = arma::ifft2 (matrix);
      Matrix<std::complex<C>> mat (fftOfM);
      return mat;
    }

template<class T>
  Matrix<T>
  Matrix<T>::submat (int row_start, int row_end, int col_start, int col_end)
  {
    arma::span row_span (row_start, row_end), col_span (col_start, col_end);
    Matrix<T> submatrix (matrix.submat (row_span, col_span));
    return submatrix;
  }

template <class T>
Matrix<double> operator*(double factor, const Matrix<T>& m)
{
  return Matrix<double>(m.matrix * factor);
}

#endif /* UAB_MATRIX */

  // template<class T>
  // std::ostream& operator<<(std::ostream& os, const Matrix<T> m)
  // {
  //   os << m.matrix;
  //   return os;
  // }
  
#endif /* _MATRIX_H */
