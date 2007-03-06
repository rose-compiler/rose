class List {
public:
  //List(int n):next(0),val(n) {}
  List* next;
  int val;
};

int main() {

  List a;
  List* x;
  List* y;
  List* t;
  List* temp;

  List* ap;
#if 0
  ap=&a;
#else
  ap=new List;
#endif

  x=ap;

  t=y; 
  y=x; 
  x=x->next;
  temp=&a;
  y->next=temp;
  temp=0;
  y->next=ap;
  y->next=0;

  t=y; 
  y=x; 
  x=x->next;
  y->next=t;

  t=0;

  return 0;
}
