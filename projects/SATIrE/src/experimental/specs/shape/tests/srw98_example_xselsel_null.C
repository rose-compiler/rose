class List {
public:
  //List(int n):next(0),val(n) {}
  List* next;
  int val;
};

int main() {
  List* a1=new List;
  List* a2=new List;
  List* a3=new List;
  List* t;

  a1->next=a2;
  a2->next=a3;

  // equivalent code for a1->next->next=0;
  t=a1->next;
  t->next=0;
  t=0;

  // forces transfer function
  a1->next->next=0;

  return 0;
}
