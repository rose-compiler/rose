class List {
public:
  List* next;
};

int main() {
  List* x;
  List* y;
  List* z;
  List* a;

  x = new List;
  y = new List;
  z = new List;
  a = new List;
  x->next=z;
  y->next=z;
  x->next=0;
  x->next=y;
  if(true) {
    z->next=x;
  } else {
    z->next=a;
  }
  return 0;
}
