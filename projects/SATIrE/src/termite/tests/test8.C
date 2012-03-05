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
 forever:
  y = new List();
  x->next=y;
  x=y;
  goto forever;
}
