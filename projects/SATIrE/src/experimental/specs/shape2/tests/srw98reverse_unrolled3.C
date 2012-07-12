class List {
public:
  List(int n):next(0),val(n) {
  }
  List* next;
  int val;
};

List* createList(int n) {
  List* listHead=0;
  List* newElement;
  {
    newElement = new List(n);
    newElement->next = listHead;
    listHead = newElement;
    newElement=0;
    n--;
  }
  {
    newElement = new List(n);
    newElement->next = listHead;
    listHead = newElement;
    newElement=0;
    n--;
  }
  {
    newElement = new List(n);
    newElement->next = listHead;
    listHead = newElement;
    newElement=0;
    n--;
  }
  return listHead;
}

List* reverseList(List* x) {
  List* y;
  List* t;
  y=0;
  {
    t=y; 
    y=x; 
    x=x->next;
    y->next=t;
  }
  {
    t=y; 
    y=x; 
    x=x->next;
    y->next=t;
  }
  {
    t=y; 
    y=x; 
    x=x->next;
    y->next=t;
  }
  t=0;
  return y;
}

int main() {
  List* acyclicList;
  List* reversedList;
  acyclicList=createList(3);
  reversedList=reverseList(acyclicList);
  acyclicList=0;
  return reversedList==0;
}
