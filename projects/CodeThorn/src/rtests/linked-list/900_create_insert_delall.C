class List {
public:
  List(int _d);

  void print() {
    printf("%d", d);
    if (n) {
      printf(", ");
      n->print();
    }
    else {
      printf("\n");
    }
  }

  List *n;
  int d;
};

List::List(int _d) : n(0), d(_d) {}

// --------------------------------------

List *create_iter(int n) {
  List *head=0;
  List *newElement;

  while (n>=0) {
    newElement = new List(n);
    newElement->n = head;
    head = newElement;
    n--;
  }

  return head;
}

// inserts an element into an ascending
// ordered list
// SagivRepsWilhelm98: insert
// changed second parameter:
//   was: pointer to existing elem
//   now: key used to create new elem
// changed return type:
//   was: void
//   now: List* to head of list
List* insert_iter(List* head, int k) {
  List* cur;
  List* tail;
  cur = head;
  while (cur->n != 0 && (cur->n->d < k)) {
    cur = cur->n;
  }
  tail = cur->n;

  List *elem = new List(k);
  elem->n = tail;
  cur->n  = elem;

  return head;
}

// deletes all elements of a list
// PavluSchordanKrall10: new testcase
void delall_iter(List *head) {
  List *t;

  while (head != 0) {
    t = head->n;
    delete head;
    head = t;
  }
}

class State {
public:
  State():iter(0),list(0),maxlen(100){}
  int iter;
  List* list;
  int maxlen;
};

void event(State* s) {
  if(s->iter==0||s->list==0) {
    s->list=create_iter(6);
  } else if(s->iter<s->maxlen) {
      insert_iter(s->list, s->iter);
  } else {
    delall_iter(s->list);
    s->list=0;
    s->iter=0;
  }
#ifdef MYPRINT
  if(s->list) {
    s->list->print();
    printf(" iter:%d max:%d\n",s->iter,s->maxlen);
  }
#endif
  s->iter++;
}

#if 0
int main(int argc, char **argv) {
  State* s=new State();
  s->maxlen=10000;
  for(int i=0;i<100000;i++) {
    event(s);
  }
  return 0;
}
#endif

