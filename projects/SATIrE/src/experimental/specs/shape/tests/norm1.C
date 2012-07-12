#define NULL ((List*)0)

class List {
public:
  List *b,*c,*d;
};

int main() {
  List *a,*norm;
  List *x1,*x2,*x3,*x4;
  
  a = new List;
  x1= new List;

  
  a->b = x1;
  norm=a->b;

  /*
  norm->c=x2;
  x2=0;
  */
  return 0;
}
