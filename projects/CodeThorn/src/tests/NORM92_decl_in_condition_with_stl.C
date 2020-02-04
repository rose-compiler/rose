#include <cassert>
#include <iostream>

struct BranchObject
{
    BranchObject(int val)
    : value(val)
    {}
    
    BranchObject(BranchObject&& other) 
    : value(other.value)
    {
      throw -1;
    }
    
    BranchObject& operator=(BranchObject&&)
    {
      throw -2;
    }
    
    operator int() { return value; }
  
  private:
    int value;
    
    BranchObject(const BranchObject& other) = delete; 
    BranchObject& operator=(const BranchObject&) = delete;
};


int main(int argc, char** argv)
{
  int cntr = 0;
  
  {
    if (BranchObject x = (++cntr, argc-1))
    {
      std::cerr << "then taken with argc-1 = " << x << std::endl;
    }
    else  
    {
      std::cerr << "else taken with argc-1 = " << x << std::endl;
    }
  }
  
  {
    if (BranchObject x = (++cntr, (argc >= 1 && argc <= 8)))
    {
      std::cerr << "then taken: argc >= 1 && argc <= 8" << std::endl;
    }
    else  
    {
      std::cerr << "else taken: argc < 1 || argc > 8" << std::endl;
    }
  }
  
  {
    switch (BranchObject x = (++cntr, argc))
    {
      default: 
        std::cerr << "default case, value was " << x << std::endl;
        break;
        
      case 0:
        std::cerr << "case 0" << std::endl;
        break;
      
      case 1:  
        std::cerr << "case 1" << std::endl;
        break;

      case 2:  
        std::cerr << "case 2" << std::endl;
        break;
    }
  }
  
  assert(cntr == 3);
  return 0;
}
