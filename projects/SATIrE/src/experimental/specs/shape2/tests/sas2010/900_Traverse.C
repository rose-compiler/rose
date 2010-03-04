// this test uses gloabl variables only
// no parameter passing

// global variables:
// g: is initialized and nullified in createList
//    before calling create_list 'g' is unitialized
// head: head is initialized with '0' and always
//    refers to the start of the list (once created)
// h: is initialized and nullified in traverseList
//    before calling traverseList 'h' is unitialized

typedef struct listru {
  struct listru *next;
} *list;
list h,g,head;
int length;

// creates a list of length 10 by inserting new
// list elements at the start of the list
void createList() {
  while(length>0) {
    g = new listru;
    g->next= head;
    head = g;
    g=0;
    length--;
  }
}

// traverses the list refered to by the global variable 'head'
void traverseList() {
  h = head;
  while(h->next!=0) {
    g = h -> next;
    h = g;
    g = 0;
  }
  h = 0;
}

int main() {
  head = 0;
  length = 10; // length of list
  createList();
  traverseList();
  return 0;
}


