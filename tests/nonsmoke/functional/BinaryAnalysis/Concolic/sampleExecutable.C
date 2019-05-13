
#include <sstream>
#include <iostream>

template <class T, class S>
T as(const S& src)
{
  std::stringstream str;  
  T                 tgt;
  
  str << src;  
  str >> tgt;
  return tgt;
}

int main(int argc, char** argv)
{
  if (argc-as<int>(argv[argc-1])) 
  {
    std::cerr << "failed" << std::endl;
    return 1;
  }
  
  return 0;
}
