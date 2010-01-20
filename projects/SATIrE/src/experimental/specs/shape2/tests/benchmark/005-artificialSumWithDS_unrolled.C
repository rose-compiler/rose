class List {
public:
  List():next(0) {;}
  List* next;
};

int main() {
  List *a, *b, *a_tail, *b_tail;
  a = new List();
  b = new List();
  a_tail = a;
  b_tail = b;

  {
    a_tail->next = new List();
    a_tail = a_tail->next;
    {
      b_tail->next = new List();
      b_tail = b_tail->next;
    }
    {
      b_tail->next = new List();
      b_tail = b_tail->next;
    }
    {
      b_tail->next = new List();
      b_tail = b_tail->next;
    }
    {
      b_tail->next = new List();
      b_tail = b_tail->next;
    }
    {
      b_tail->next = new List();
      b_tail = b_tail->next;
    }
  }
  {
    a_tail->next = new List();
    a_tail = a_tail->next;
    {
      b_tail->next = new List();
      b_tail = b_tail->next;
    }
    {
      b_tail->next = new List();
      b_tail = b_tail->next;
    }
    {
      b_tail->next = new List();
      b_tail = b_tail->next;
    }
    {
      b_tail->next = new List();
      b_tail = b_tail->next;
    }
  }
  {
    a_tail->next = new List();
    a_tail = a_tail->next;
    {
      b_tail->next = new List();
      b_tail = b_tail->next;
    }
    {
      b_tail->next = new List();
      b_tail = b_tail->next;
    }
    {
      b_tail->next = new List();
      b_tail = b_tail->next;
    }
  }
  {
    a_tail->next = new List();
    a_tail = a_tail->next;
    {
      b_tail->next = new List();
      b_tail = b_tail->next;
    }
    {
      b_tail->next = new List();
      b_tail = b_tail->next;
    }
  }
  {
    a_tail->next = new List();
    a_tail = a_tail->next;
    {
      b_tail->next = new List();
      b_tail = b_tail->next;
    }
  }
  {
    a_tail->next = new List();
    a_tail = a_tail->next;
  }

  //a_tail->next = b;
  //int len = length(a)-2;
}
