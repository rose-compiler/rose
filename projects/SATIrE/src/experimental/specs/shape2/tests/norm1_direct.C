#define NULL ((List*)0)

class List {
public:
  List *n1,*n2,*n3;
};

int main() {
  List *l;
  
  l = new List;
  l->n1 = new List;
  l->n1->n2 = new List;
  l->n1->n2->n3 = new List;

  l = l->n1->n2->n3;

  return 0;
}
