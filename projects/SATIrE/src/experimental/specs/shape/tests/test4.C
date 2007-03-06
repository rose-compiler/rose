class List {
public:
  List* next;
};

int main() {
  List* x;
  List* y;
  List* s;
  
  x=new List();
  s=x;
  int i=0;
 redo:
  y = new List();
  x->next=y;
  x=y;
  if(i<5) goto redo;
  return 0;
}
