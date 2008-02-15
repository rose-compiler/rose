// cc.in73
// inheriting from a typedef'd class and trying to call
// the superclass dtor in the member init

class txList {};
typedef txList List;

class Stack : private List {
  Stack();
};

Stack::Stack() : List()
{}
