class List {
public:
  List(int n):next(0),val(n) {}
  List* next;
  int val;
};

List* createList(int n) {
  List* listHead=0;
  List* newElement;
  while(n>0) {
    newElement = new List(n);
    newElement->next = listHead;
    listHead = newElement;
    n--;
  }
  return listHead;
}

int main() {
  List* acyclicList;
  acyclicList=createList(10);
  return 0;
}
