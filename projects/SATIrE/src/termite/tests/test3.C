class List {
public:
  List* next;
};

int main() {
  List* x;
  List* y;
  List* z;

  x = new List;
  y = new List;
  z = new List;
  y->next=x;
  z->next=x;
  return 0;
}
