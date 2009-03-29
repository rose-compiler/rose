class List {
public:
  List():next(0) {;}
  List* next;
};

int main() {
  List *a, *b, *a_tail, *b_tail;
  int i,j,n;
  n = 7;
  a = new List();
  b = new List();
  a_tail = a;
  b_tail = b;
  i = n;
  j = n;

  while (i > 0) {
    a_tail->next = new List();
    a_tail = a_tail->next;
    i = i - 1;
    j = i;
    while (j > 0) {
      b_tail->next = new List();
      b_tail = b_tail->next;
      j = j - 1;
    }
  }
  
  //a_tail->next = b;
  //int len = length(a)-2;
}
