#define NULL ((Node*)0)

// Implementation of the DSW-algorithm from: THE ART OF COMPUTER PROGRAMMING
// Third Edition, p-> 418-419

// defines a node in a list structure
class Node {
  public:
  //string name;   // identification
  bool ATOM;     // true, if node has no sons
  bool MARK;     // true, if node has already been visited
  class Node *ALINK;   // one son of the node
  class Node *BLINK;   // another son of the node

  Node(/*string n*/) : ATOM(false), MARK(false), ALINK(NULL), BLINK(NULL) {}
};

// variables-declaration
class Node *a,*b,*c,*d,*e,*T,*P,*Q;
bool done;

// the algorithm consists of six steps: E1 - E6
static void traversal(Node *P0) {
  T = NULL;
  P = P0;
  done = false;

  // << "Startnode: " << P->name << endl;
  while (! done) {
    while (P != NULL && !P->MARK) {
      P->MARK = true;
      if (!P->ATOM) {
        Q = P->ALINK;
        if (Q != NULL) {
          // << "ALINK from " << P->name << " is: " << Q->name << endl;
        } else {
          // << "ALINK from " << P->name << " is: null" << endl;
        }
        P->ALINK = T;
        T = P;
        P = Q;
      }
    }
    while (T != NULL && T->ATOM) {
      T->ATOM = false;
      Q = T->BLINK;
      T->BLINK = P;
      P = T;
      if (P != NULL) {
        // << "Return: " << P->name << endl;
      } else {
        // << "Return: NULL" << endl;
      }
      T = Q;
    }
    if (T == NULL) {
      done = true;
    } else {
      T->ATOM = true;
      Q = T->ALINK;
      T->ALINK = P;
      P = T->BLINK;
      if (P != NULL) {
        // << "BLINK from " << T->name << " is: " << P->name << endl;
      } else {
        // << "BLINK from " << T->name << " is: NULL" << endl;
      }
      T->BLINK = Q;
    }
  }
}


// to show the results
int main(int argc, char **argv) {

  // example of a tree structure of figure 39, p-> 419
  a = new Node(/*"a"*/);
  b = new Node(/*"b"*/);
  c = new Node(/*"c"*/);
  d = new Node(/*"d"*/);
  e = new Node(/*"e"*/);

  a->ALINK = b;
  a->BLINK = c;
  c->ALINK = b;
  c->BLINK = d;
  d->ALINK = e;
  d->BLINK = d;
  e->BLINK = c;
  b->ATOM  = true;

  traversal(a);

  return 0;
}

