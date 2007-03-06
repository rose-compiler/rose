class List {
public:
  List* next;
};

List* createList(int n) {
  List* listHead=0;
  List* newElement;
  while(n>0) {
    newElement = new List;
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
  while(x!=0) {
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
  List* acyclicList2;
  List* reversedList;
  acyclicList=createList(10);
  acyclicList2=acyclicList->next; // pointer to 2nd element
  reversedList=reverseList(acyclicList);
  acyclicList=0;
  return reversedList==0;
}
