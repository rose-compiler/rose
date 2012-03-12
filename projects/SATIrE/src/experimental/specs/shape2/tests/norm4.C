#define NULL ((List*)0)

class List {
public:
  List *n1,*n2,*n3;
};

int main() {
  List *l, *lt1, *lt2, *lt3;
  List *m, *mt1, *mt2, *mt3;
  
  l = new List;
  m = new List;

  lt1 = new List;
  lt2 = new List;
  lt3 = new List;
  
  mt1 = new List;
  mt2 = new List;
  mt3 = new List;
  
  l->n1 = lt1;
  l->n1->n2 = lt2;
  l->n1->n2->n3 = lt3;

  m->n1 = mt1;
  m->n1->n2 = mt2;
  m->n1->n2->n3 = mt3;

  l->n1->n2->n3 = m->n1->n2->n3;

  return 0;
}
