class List {
public:
  //List(int n):next(0),val(n) {}
  List* next;
  int val;
};

int main() {
  List a;
  List b;
  List* ap;
  List** app;
  List* bp;
  List c;
  List* cp;

  ap=&a;
  app=&ap;
  bp=&b;
  ap->next=bp;
  cp=&c;
  bp->next=cp;


#if 1
  List* x;
  List* y;
  List* t;
  x=ap;

  t=y; 
  y=x; 
  x=x->next;
  y->next=t;

  t=y; 
  y=x; 
  x=x->next;
  y->next=t;

  t=y; 
  y=x; 
  x=x->next;
  y->next=t;

  t=y; 
  y=x; 
  x=x->next;
  y->next=t;

  t=y; 
  y=x; 
  x=x->next;
  y->next=t;

  t=y; 
  y=x; 
  x=x->next;
  y->next=t;

  t=0;
#else
  List* revList;
  revList=reverseList(ap);
#endif
  return 0;
}
