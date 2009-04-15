#define NULL ((SingleLinkedList*)0)
using namespace std;

// implementation of an acyclic single-linked and sorted list
class SingleLinkedList {
  public:
    int element;
    class SingleLinkedList *next;
};

// shows the elements of the acyclic single-linked list
void ShowList(SingleLinkedList *list) {
  SingleLinkedList *l = list;
  while (l != NULL) {
    // << l->element << endl;
    l = l->next;
  }
}

// reverses a list
SingleLinkedList *ReverseList(SingleLinkedList *list) {
  SingleLinkedList *l = list;
  SingleLinkedList *y = NULL, *t;

  while (l != NULL) {
    t = y;
    y = l;
    l = l->next;
    y->next = t;
  }
  t = NULL;
  return y;
}

// creates a new acyclic single-linked list of a given length
SingleLinkedList *CreateList(int length) {
  int i = length;
  SingleLinkedList *list = NULL;
  SingleLinkedList *dummylist = NULL;

  while (i > 0) {
    list = new SingleLinkedList();
    list->element = i;
    list->next = dummylist;
    dummylist = list;
    i--;
  }
  return (list);
}

// inserts a new element in the existing list      
SingleLinkedList *InsertInList(SingleLinkedList *list, SingleLinkedList *e) {
  SingleLinkedList *y = list;
    
  if (y->element < e->element) {
    while (y->element < e->element && y->next != NULL) {
      y = y->next;
    }
    e->next = y->next;
    y->next = e;
  } else {
    e->next = list;
    list = e;
  }
  return list;
}

// removes an element from the single-linked list
SingleLinkedList *RemoveFromList(SingleLinkedList *list, SingleLinkedList *e) {
  SingleLinkedList *l = list;
  if (l->element != e->element) {
    while (l->next != NULL && e->element != l->next->element) {
      l = l->next;
    }
    if (l->next == NULL) {
      // << "no element to remove" << endl;
    } else {
      l->next = l->next->next;
    }
  } else {
    list = list->next;
  }
  return list;
}

// to show the results
int main(int argc, char **argv) {

  SingleLinkedList *insertelement = new SingleLinkedList();
  insertelement->element = 0;
  insertelement->next = NULL;

  SingleLinkedList *removeelement = new SingleLinkedList();
  removeelement->element = 3;
  removeelement->next = NULL;

  // << "CreateList" << endl;
  SingleLinkedList *list = CreateList(6);
  ShowList(list);
  // << endl;
  // << endl;

  // << "InsertInList: " << insertelement->element << endl;
  list = InsertInList(list, insertelement);
  ShowList(list);
  // << endl;
  // << endl;

  // << "ReverseList" << endl;
  list = ReverseList(list);
  ShowList(list);
  // << endl;
  // << endl;

  // << "ReverseList" << endl;
  list = ReverseList(list);
  ShowList(list);
  // << endl;
  // << endl;

  // << "RemoveFromList: " << removeelement->element << endl;
  list = RemoveFromList(list, removeelement);
  ShowList(list);
  // << endl;
  // << endl;
}
