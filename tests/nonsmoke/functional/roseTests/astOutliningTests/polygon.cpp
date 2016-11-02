#include <iostream>
#include <dlfcn.h>
#include <cmath>
class polygon {
  protected:
    double side_length_;
  public:
    polygon()
      : side_length_(0) {}

    void set_side_length(double side_length) {
      side_length_ = side_length;
    }

    virtual double area() const = 0;
};

class triangle : public polygon {
  public:
    virtual double area() const {
      double result;
#pragma rose_outline      
      result = side_length_ * side_length_ * sqrt(3) / 2; 
      return result;
    }
};

int main() {
  using std::cout;
  using std::cerr;
  polygon* poly = new triangle;
  poly->set_side_length(7.0);
  std::cout<<"The area is:"<<poly->area()<<std::endl;
  delete poly;
  return 0;
}

