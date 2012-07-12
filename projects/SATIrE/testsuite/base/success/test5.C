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
 redo:
  y = new List();
  x->next=y;
  switch(i) {
  case 0:
    x->val=100;
    break;
    x->val+=5; // unreachable
    break;
  default:
    x->val=i;
  }
  x=y;
    
  if(i<5) goto redo;
  return 0;
}
