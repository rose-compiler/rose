#include <iostream>
#include <cmath>

class triangle{
  protected:
    double side_length_;
  public:
    triangle():side_length_(0) {}

    void set_side_length(double side_length) 
    {
      side_length_ = side_length;
    }
    double area() const 
    {
      double result;
#pragma rose_outline      
      result = side_length_ * side_length_ * sqrt(3) / 2; 
      return result;
    }
};

int main() {
  using std::cout;
  using std::cerr;
  triangle* tri= new triangle;
  tri->set_side_length(7.0);
  std::cout<<"The area is:"<<tri->area()<<std::endl;
  delete tri;
  return 0;
}

