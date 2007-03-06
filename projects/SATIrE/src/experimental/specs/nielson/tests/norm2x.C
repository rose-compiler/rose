#define NULL ((List*)0)

class List {
public:
  List *n1,*n2,*n3;
};

int main() {
  List *l, *t1, *t2, *t3;
  List *$norm_6_l0;
  List *$norm_5_l0, *$norm_5_l1;
  
  l = new List;

  t1 = new List;
  t2 = new List;
  t3 = new List;
 
  // stmt 7 in norm1.C
  // l->n1 = t1;
  l->n1 = t1;

  // stmt 6 in norm1.C
  // l->n1->n2 = t2;
  $norm_6_l0 = l->n1;
  $norm_6_l0->n2 = t2;

  // stmt 5 in norm1.C
  // l->n1->n2->n3 = t3;
  $norm_5_l1 = l->n1;
  $norm_5_l0 = $norm_5_l1->n2;
  $norm_5_l0->n3 = t3;

  return 0;
}
