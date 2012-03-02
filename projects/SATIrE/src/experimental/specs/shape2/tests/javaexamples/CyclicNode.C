#define NULL ((CyclicNode*)0)
using namespace std;

// implementation of an acyclic single-linked list, 
// but the last node is a cyclic node
class CyclicNode {
  public:
    int element;
    class CyclicNode *next;
};

// shows the elements of the list
void ShowCyclicNode(CyclicNode *cnode) {
  CyclicNode *l = cnode;

  while (l != l->next ) {
    //cout << l->element << endl;
    l = l->next;
  }
  //cout << l->element << endl;
}

// creates a new list, it is an object of type CyclicNode
// and returns the created list
CyclicNode *CreateCyclicNode(int length) {
  int i = length;
  CyclicNode *cnode = NULL;
  CyclicNode *dummycnode;
  CyclicNode *returncnode;
    
  while (i > 0) {
    dummycnode = new CyclicNode();
    dummycnode->element = i;
    dummycnode->next = cnode;
    cnode = dummycnode;
    i--;
  }
  returncnode = cnode;

  // make the last node a cyclic node
  while (cnode->next != NULL)
    cnode = cnode->next;
  cnode->next = cnode;
    
  return (returncnode);
}

// inserts a new element in the existing ordered list
CyclicNode *InsertInCyclicNode(CyclicNode *cnode, CyclicNode *e) {
  CyclicNode *y = cnode;
      
  while (y->element < e->element && y->next != y) {
    y = y->next;
  }
    
  if (y->next == y) {
    y->next = e;
    e->next = e;
  } else if (y->next != y && y->element == e->element) {
    e->next = y->next;
    y->next = e;
  }
  return cnode;
}

// removes the element of type CyclicNode from the cnode of type CyclicNode
// element e must be in the cnode
CyclicNode *RemoveFromCyclicNode(CyclicNode *cnode, CyclicNode *e) {
  CyclicNode *l = cnode;
    
  if (l->element != e->element) {
    while (e->element != l->next->element) 
      l = l->next;
        
    if (l->next->next == l->next)
      l->next = l;
    else
      l->next = l->next->next;
  }
  else
    cnode = cnode->next;
  return cnode;
}

// main muss immer public und static sein
int main (int argc, char **argv) {
  CyclicNode *insertelement = new CyclicNode();
  insertelement->element = 9;
  insertelement->next = NULL;

  CyclicNode *removeelement = new CyclicNode();
  removeelement->element = 6;
  removeelement->next = NULL;

  CyclicNode *cnode = CreateCyclicNode(6);
  //cout << "CreateCyclicNode" << endl;
  ShowCyclicNode(cnode);
  //cout << endl;
  //cout << endl;

  cnode = InsertInCyclicNode(cnode, insertelement);
  //cout << "InsertInCyclicNode: " << insertelement->element << endl;
  ShowCyclicNode(cnode);
  //cout << endl;
  //cout << endl;

  cnode = RemoveFromCyclicNode(cnode, removeelement);
  //cout << endl;
  //cout << endl;
  //cout << "RemoveFromCyclicNode: = " << removeelement->element << endl;
  ShowCyclicNode(cnode);

  return 0;
}
