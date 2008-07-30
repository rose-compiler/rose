#ifndef RESULT_H
#define RESULT_H

#include <assert.h>

#include <dbheaders.h>
#include <Row.h>

// DQ (12/30/2005): This is a Bad Bad thing to do (I can explain)
// it hides names in the global namespace and causes errors in 
// otherwise valid and useful code. Where it is needed it should
// appear only in *.C files (and only ones not included for template 
// instantiation reasons) else they effect user who use ROSE unexpectedly.
// using namespace std;

class Result {

 private:
  Matrix dataRows;
  Row headerRow;
  int nRows, nCols;
  std::vector <std::string> colTypes;

 public:
  Result (char **d, int nRow, int nCol, std::vector <std::string> *types);
  Result(const Result &);
  void showResult();
  // return a wrapper for vector <string>
  Row operator[] (int i)
  {
    assert (i >= 0 && i < nRows);
    return dataRows[i];
  }
  // return schema
  Row header () { return headerRow; }
  // return sizes
  int rows () { return nRows; }
  int cols () { return nCols; }

  class iterator {
  public:
    iterator (int r, const Matrix *rData, int Init = 0) : currentNo (Init), rowsNo (r) { rows = rData; }
    // prefix
    iterator& operator++ ()
    {
      if (currentNo < rowsNo)
	currentNo++;
      return *this;
    
    }
    // postfix
    iterator operator++ (int)
    {
      iterator tmp = *this;
      currentNo++;
      return tmp;
    }
    iterator operator- (int i)
    {
      if (currentNo < i)
      {
	std::cerr << "Runtime error, iterator substraction out of bounds\n";
	exit(1);
      }
      currentNo -= i;
    }
    bool operator != (const iterator& rSide)
    {
      if (this->rows == rSide.rows && currentNo == rSide.currentNo)
	return false;
      return true;
    }
    // dereferencing
    Row operator* ()
    {
      return (*rows)[currentNo];
    }
  private:
    int currentNo, rowsNo;
    const Matrix *rows;
  };
 
  iterator begin() const { return iterator(nRows, &dataRows, 0); }
  iterator end() const { return iterator(nRows, &dataRows, nRows); }
};

#endif
