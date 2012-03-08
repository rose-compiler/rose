#define NULL ((N*)0)

class N {
public: N *sel;
};

int main() {
  int flag = 42;
  N *x, *y, *t;

  x = new N;
  y = new N;
  t = new N;

  if (flag == 42) {
    x->sel = t;
  } else {
    y->sel = t;
  }
}

