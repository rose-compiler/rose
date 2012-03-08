#define NULL ((List*)0)

class List {
public:
  List *next;
};

int main() {
  List *x,*y,*t1,*t2,*t3,*t4;
  
  x = new List;

  x->next=x;

  t1 = new List;
  t2 = new List;
  y = new List;
  
  x->next=t1;
  t1->next=t2;  
  t1=NULL;
  t2=NULL;

  // normalization
  t1=x->next;  
  t2=t1->next;  
  y=t2->next;



  return 0;
}
