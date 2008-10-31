#ifndef ITEM_R_H
#define ITEM_R_H


class Item {
 public:
  Item(bool function, SgAsmStatement* statement,
       int functionSize, int resolved, int row):function(function),
    statement(statement),functionSize(functionSize),
    resolved(resolved),row(row){
    fg = QColor(128,128,128);
    bg = QColor(128,128,128);
}

  bool function;
  SgAsmStatement* statement;
  int functionSize;
  int resolved;
  int row;
  // is this a added or removed instruction (clone detection)

  QColor bg;
  QColor fg;

};



#endif
