
#ifndef __KLT_TESTS_DATA_MANAGER_HPP__
#define __KLT_TESTS_DATA_MANAGER_HPP__

namespace DataManager {
  float * init(unsigned dim, unsigned long * size);
  float * load(unsigned dim, unsigned long * size, char * filename);
  float compare(unsigned dim, unsigned long * size, float * a, float * b);
}

#endif /* __KLT_TESTS_DATA_LOADER_HPP__ */
