#ifndef ITEM_R_H
#define ITEM_R_H
#include "rose.h"

class Item {
 public:
  Item(bool function, SgAsmStatement* statement,
       int functionSize, int resolved):function(function),
    statement(statement),functionSize(functionSize),
    resolved(resolved){}

  bool function;
  SgAsmStatement* statement;
  int functionSize;
  int resolved;
};

#endif
