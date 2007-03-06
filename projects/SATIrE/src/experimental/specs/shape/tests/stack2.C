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
  List* c;

  ap=&a;
  app=&ap;
  bp=&b;
  ap->next=bp;
  c=new List;
  bp->next=c;

  *app=0;
  return 0;
}
