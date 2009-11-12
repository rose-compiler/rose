struct List {
  List(int n):next(0),val(n) {
  }
  List* next;
  int val;
};

void insert(List* x, List* e) {
  List* y;
  List* t;
  y=x;
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

int main() {
  List* acyclicList=new List(-1);
  int vals[]={2,5,3,7,4,-1};
  int i=0;
  while(vals[i]!=-1) {
    List* e;
    e=new List(vals[i]);
    insert(acyclicList,e);
    i++;
  }
}

  
