// array of structs and pointer artihmetic on array elements.

struct s1 {
  int x;
};

struct s1 a[10];
 
void main() {
  struct s1* ap = {&a[0]};
  (ap +1)->x=1;
}
