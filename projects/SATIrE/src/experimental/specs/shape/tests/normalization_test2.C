#define NULL ((List*)0)

class List {
public:
  List* next;
};

int main() {
  List *a0, *a1, *a2, *a3;
  List *x0, *x1, *x2, *x3;

  a0 = new List;
  a1 = new List;
  a2 = new List;
  a3 = new List;

  a0->next = a1;
  a1->next = a2;
  a2->next = a3;
  
  //a->next = new List;
  //a->next->next = new List;
  //a->next->next->next = new List;

  x0 = new List;
  x1 = new List;
  x2 = new List;
  x3 = new List;

  x0->next = x1;
  x1->next = x2;
  x2->next = x3;

  a0->next->next->next = x0->next->next->next;

  return 0;
}
