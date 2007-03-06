#define NULL ((List*)0)

class List {
public:
  List *n1,*n2,*n3;
};

int main() {
  List *l, *t1, *t2, *t3;
  
  l = new List;

  t1 = new List;
  t2 = new List;
  t3 = new List;
  
  l->n1 = t1;
  l->n1->n2 = t2;
  l->n1->n2->n3 = t3;

  l = l->n1->n2->n3;

  return 0;
}
