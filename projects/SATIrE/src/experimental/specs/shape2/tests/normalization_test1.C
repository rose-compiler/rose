#define NULL ((List*)0)

class List {
public:
  List *b,*c,*d;
};

int main() {
  List *a;
  
  a = new List;
  a->b = new List;
  a->b->c = new List;
  a->b->c->d = new List;

  a = a->b->c->d;

  return 0;
}
