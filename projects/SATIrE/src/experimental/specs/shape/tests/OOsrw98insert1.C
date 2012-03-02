// Modification of srw98insert.C
// * Methods are declared in the class declaration
// * tests aliasing of 'this' in the same class and list inserts
class List {
public:
  List(int n):next(0),val(n) {
  }
  void insert(int value) {
    List* e;
    e=new List(value);
    List* y;
    List* t;
    y=this;
    while(y->next!=0 && (y->next->val < e->val)) {
      y=y->next;
    }
    t=y->next;
    e->next=t;
    y->next=e;
    t=0;
    e=0;
    y=0;
  }
private:
  List* next;
  int val;
};

int main() {
  List* acyclicList=new List(-1);
  int vals[]={2,5,3,7,4,-1};
  int i=0;
  while(vals[i]!=-1) {
    acyclicList->insert(vals[i]);
    i++;
  }
}
