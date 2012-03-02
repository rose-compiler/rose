#include "printList.h"

void printListInfo(char* s, List* x) {
  std::cout << s;
  printList(x);
}

void printList(List* x) {
  std::cout << "[";
  while(x!=0) {
    std::cout << x->val;
    x=x->next;
    if(x!=0)
      std::cout << ",";
  }
  std::cout << "]" << std::endl;;
}

