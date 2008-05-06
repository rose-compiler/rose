#include <iostream>

void thomas(void* n, std::string desc) {
  if (n==NULL) {
    std::cerr << " Error " << desc << std::endl;
  }
}
