class List {
public:
  //List(int n):next(0),val(n) {}
  List* next;
  int val;
};

int main() {

  List* x;
  List* y;
  List* z;
  List* t;

  x=new List;
  y=new List;
  t=new List;

  x->next=t;
  y->next=t;
  t=0;

  z=y->next;
  z->next=0;
  z=0;

  return 0;
}
