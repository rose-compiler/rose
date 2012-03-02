class List {
public:
  List* next;
};

int main() {
  List* x;
  List* y;
  List* z;
  List* t;

  x = new List;
  y = new List;
  z = new List;
  x->next=y;
  y->next=z;
#if 1
  z->next=x->next;
#else
  t=x->next;
  z->next=t;
  t=0;
#endif
  return 0;
}
