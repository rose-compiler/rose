#ifdef PRINT_RESULT
#include <cstdio>
#endif

class List {
public:
  List(int _d) : next(0), val(_d) {}

  void print() {
#ifdef PRINT_RESULT
    printf("%d", val);
    if (next) {
      printf(", ");
      next->print();
    }
    else {
      printf("\n");
    }
#endif
  }

  List* next;
  int val;
};


List* create_iter(int n) {
  List* head=0;
  List* newElement;

  while (n>=0) {
    newElement = new List(n+1);
    newElement->next = head;
    head = newElement;
    n--;
  }

  return head;
}

// inserts an element into an ascending
// ordered list
List* insert_iter(List* head, int k) {
  List* cur;
  List* tail;
  cur = head;
  while (cur->next != 0 && (cur->next->val < k)) {
    cur = cur->next;
  }
  tail = cur->next;

  List *elem = new List(k);
  elem->next = tail;
  cur->next  = elem;

  return head;
}

// deletes all elements of a list
void delall_iter(List** head0) {
  List* t;
  List* head =*head0;
  while (head != 0) {
    t = head->next;
    delete head;
    head = t;
  }
  *head0=0;
}

int compute_sum(List* head) {
  int sum=0;
  List *t;
  while (head != 0) {
    t = head->next;
    sum+=head->val;
    head = t;
  }
  return sum;
}

int main(int argc, char **argv) {
  List *a = create_iter(6);
#ifdef PRINT_RESULT
  a->print();
#endif
  int sum1=compute_sum(a);
  insert_iter(a, 3);
  int sum2=compute_sum(a);
  delall_iter(&a);
  int sum3=compute_sum(a);
  
#ifdef PRINT_RESULT
  printf("%d %d %d\n",sum1,sum2,sum3);
#endif
  return (sum1==28 && sum2==31 && sum3==0);
}
