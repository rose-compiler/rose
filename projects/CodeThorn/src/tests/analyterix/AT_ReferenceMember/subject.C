struct A18;

struct A18 {
  int val;
  A18& next;
  A18() : next(*this) { }
  A18(A18& _next) : next(_next) { }
};

class A27
{
public:
    int i;
    static float& f;
};

void foo18() {
  A18 last;
  A18 list(last);
  list.next.val = list.val;
  
  float* f_ptr = &A27::f;
}
