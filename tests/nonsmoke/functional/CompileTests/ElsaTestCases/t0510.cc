// t0510.cc
// a class befriending itself
// from Sohail Somani

template <class Y>
class test {
  template <class T> 
  friend class test;
};

test<int> ff;
