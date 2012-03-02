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
  
  //(**app).next=0; // (*app)->next

  return 0;
}
