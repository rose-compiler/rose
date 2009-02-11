#ifndef ITEM_R_H
#define ITEM_R_H
//#include "rose.h"

class Item {
 public:
  Item(rose_addr_t addr, SgNode* statement,
       int resolvedColor, int row, int length, int realByteSize, int pos,
       std::string comment, int lineNr):
  addr(addr),statement(statement),
    resolvedColor(resolvedColor),
    row(row),length(length),realByteSize(realByteSize),pos(pos),
    comment(comment),lineNr(lineNr){
    fg = QColor(128,128,128);
    bg = QColor(128,128,128);
}

  rose_addr_t addr;
  SgNode* statement;
  int resolvedColor;
  int row;
  int length;
  int realByteSize;
  int pos;
  std::string comment;
  int lineNr;
  QColor bg;
  QColor fg;

};



#endif
