class List {
public:
  //List(int n):next(0),val(n) {}
  List* next;
  int val;
};

int main() {
  List *a,**ap, ***app,*cp,**ap2;
  a=new List;
  List c;
  cp=&c;
  ap=&a;
  app=&ap;
  a->next=cp;
  return 0;
}
