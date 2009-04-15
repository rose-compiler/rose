#define NULL ((Node*)0)
using namespace std;

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

  Node(/*string n*/) {
    //name  = n;
    ATOM  = false;
    MARK  = false;
    ALINK = NULL;
    BLINK = NULL;
  }
};

class DSW {
  public:
  // variables-declaration
  static class Node *a,*b,*c,*d,*e,*T,*P,*Q;
  static bool done;

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
};

// initialisation of static members
class Node *DSW::a = NULL;
class Node *DSW::b = NULL;
class Node *DSW::c = NULL;
class Node *DSW::d = NULL;
class Node *DSW::e = NULL;
class Node *DSW::T = NULL;
class Node *DSW::P = NULL;
class Node *DSW::Q = NULL;
bool DSW::done = false;

// to show the results
int main(int argc, char **argv) {
  // example of a tree structure of figure 39, p-> 419
  DSW::a = new Node(/*"a"*/);
  DSW::b = new Node(/*"b"*/);
  DSW::c = new Node(/*"c"*/);
  DSW::d = new Node(/*"d"*/);
  DSW::e = new Node(/*"e"*/);

  DSW::a->ALINK = DSW::b;
  DSW::a->BLINK = DSW::c;
  DSW::c->ALINK = DSW::b;
  DSW::c->BLINK = DSW::d;
  DSW::d->ALINK = DSW::e;
  DSW::d->BLINK = DSW::d;
  DSW::e->BLINK = DSW::c;
  DSW::b->ATOM  = true;

  DSW::traversal(DSW::a);
  return 0;
}

