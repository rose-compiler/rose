class List {
public:
  List* next;
  int val;
};

int main() {
  List* x;
  List* y;
  List* s;
  
  x=new List();
  s=x;
  int i=0;
 loop:
  y = new List();
  x->next=y;
  x=y;
  if(i>4) goto afterLoop;
  goto loop;
 afterLoop:
  return 0;
}
