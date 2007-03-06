class List {
public:
  //List(int n):next(0),val(n) {}
  List* next;
  int val;
};

List* createList(int n) {
  List* listHead=0;
  List* newElement;
  while(n>0) {
    newElement = new List;
    newElement->next = listHead;
    listHead = newElement;
    n--;
  }
  return listHead;
}

int main() {
  List* x;
  List* t;
  List* z;
  x=createList(10);

  if(0) {
  // x->sel->sel=0;
  t=x->next;
  t->next=0;
  t=0;
  } else {
    z=x->next;
    t=x->next;
    t->next=0;
    t=0;
    z=0;
  }
  return 0;
}
