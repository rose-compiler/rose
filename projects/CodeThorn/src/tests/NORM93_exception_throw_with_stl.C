#include <exception>
#include <iostream>

template <class Error = std::runtime_error>
void errorIf(bool isError, std::string msg)
{
  if (isError) 
    throw Error(msg);
}

template <class T>
T& deref(T* ptr)
{
  errorIf<>(ptr == nullptr, "null pointer dereference");
  
  return *ptr;
}

size_t sumPositiveNumbers(int* aa, int* zz) try
{
  errorIf<std::runtime_error>(aa == nullptr || zz == nullptr, "illegal sequence");
  
  size_t total = 0;
  
  while (zz != aa)
  {
    int val = deref(--zz);
    
    errorIf<std::logic_error>(val <= 0, "value less or equal to zero");
    total += val;
  } 
  
  return total;
}
catch (const std::runtime_error& ex)
{
  std::cerr << "caught exception: " << ex.what() << std::endl;
  return 0;
}
catch (...)
{
  std::cerr << "propagate exception" << std::endl;
  throw ;
}

int main() throw ()
{
  try
  {
    int    numbers[] = {2, 4, 6, 8, 1, 3, 5, 9};
    size_t numelems  = sizeof(numbers) / sizeof(numbers[0]);
    size_t total = sumPositiveNumbers(numbers, numbers + numelems);
    
    std::cout << "sum = " << total << std::endl;
  }
  catch (...)
  {}
  
  try
  {
    int    numbers[] = {-2, 4};
    size_t numelems  = sizeof(numbers) / sizeof(numbers[0]);
    size_t total = sumPositiveNumbers(numbers, numbers + numelems);
    
    std::cout << "sum = " << total << std::endl;
  } 
  catch (...)
  {}
  
  try
  {
    size_t total = sumPositiveNumbers(nullptr, nullptr);
    
    std::cout << "sum = " << total << std::endl;
  } 
  catch (...)
  {}
  
  return 0;
}

