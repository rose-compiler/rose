#ifndef ITEM_R_H
#define ITEM_R_H
#include "rose.h"

class Item {
 public:
  Item(bool function, SgNode* statement,
       int functionSize, int resolved, int row, int length, int pos,
       std::string comment, int lineNr):function(function),
    statement(statement),functionSize(functionSize),
    resolved(resolved),row(row),length(length),pos(pos),comment(comment),lineNr(lineNr){
    fg = QColor(128,128,128);
    bg = QColor(128,128,128);
}

  bool function;
  SgNode* statement;
  int functionSize;
  int resolved;
  int row;
  
  int length;
  int pos;
  std::string comment;
  int lineNr;
  // is this a added or removed instruction (clone detection)

  QColor bg;
  QColor fg;

};



#endif
