/*
 * A toy loop data structure demonstrating a thin client of abstract handles:
 * A simplest loop tool which keeps a tree of loops in a file
 */
#ifndef my_loop_INCLUDED
#define my_loop_INCLUDED

#include <string>
#include <vector>
class MyLoop 
{
public:  
  std::string sourceFileName;
  size_t line_number;
  std::string loop_code;
  std::vector<MyLoop*> children; 
};

#endif
