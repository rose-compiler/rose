#define NULL ((CyclicSingleLinkedList*)0)
using namespace std;

// implementation of an cyclic single-linked list
class CyclicSingleLinkedList {
  public:
    int element;
    class CyclicSingleLinkedList *next;
};

// shows the elements of the list
void ShowCyclicSingleLinkedList(CyclicSingleLinkedList *list) {
	CyclicSingleLinkedList *l = list->next;
	CyclicSingleLinkedList *act = list;

	while (l->element != act->element) {
    // << l->element << endl;
	  l = l->next;
	}
}

// creates a new list of type List and returns the created list
CyclicSingleLinkedList *CreateCyclicSingleLinkedList(int length) {
	int i = length;
	CyclicSingleLinkedList *list = NULL;
	CyclicSingleLinkedList *dummyqueue;
	CyclicSingleLinkedList *returnqueue;

	while (i > 0) {
	  i--;
	  dummyqueue = new CyclicSingleLinkedList();
	  dummyqueue->element = i;
	  dummyqueue->next = list;
	  list = dummyqueue;
	}
	returnqueue=list;
	while (list->next != NULL)
	  list = list->next;
	list->next = returnqueue;

	return (returnqueue);
}
    
// inserts a new element in the existing list     
CyclicSingleLinkedList *InsertInCyclicSingleLinkedList(CyclicSingleLinkedList *list, CyclicSingleLinkedList *e) {
	CyclicSingleLinkedList *y = list->next;
	CyclicSingleLinkedList *s = list;
	CyclicSingleLinkedList *z;
  CyclicSingleLinkedList *t;
	
	while (y->element < e->element && y->next != s) {
	  z = y->next;
	  y = z;
	}

	e->next = y->next;
	y->next = e;

	return list;
}

// removes an element from the list
// element e must be in the list
CyclicSingleLinkedList *RemoveFromCyclicSingleLinkedList(CyclicSingleLinkedList *list, CyclicSingleLinkedList *e) {
	CyclicSingleLinkedList *l = list;
	while (l->next != NULL && e->element != l->next->element) {
	  l = l->next;
	}
	l->next = l->next->next;
	return list;
}

// to show the results
int main(int args, char **argv) {
  CyclicSingleLinkedList *insertelement = new CyclicSingleLinkedList();
  insertelement->element = 14;
  insertelement->next = NULL;

  CyclicSingleLinkedList *removeelement = new CyclicSingleLinkedList();
  removeelement->element = 1;
  removeelement->next = NULL;

  CyclicSingleLinkedList *list = CreateCyclicSingleLinkedList(6);
  // << "CreateCyclicSingleLinkedList" << endl;
  ShowCyclicSingleLinkedList(list);
  // << endl;
  // << endl;

  list = InsertInCyclicSingleLinkedList(list, insertelement);
  // << "InsertInCyclicSingleLinkedList" << endl;
  ShowCyclicSingleLinkedList(list);
  // << endl;
  // << endl;

  list = RemoveFromCyclicSingleLinkedList(list, removeelement);
  // << endl;
  // << endl;
  // << "RemoveFromCyclicSingleLinkedList" << endl;
  ShowCyclicSingleLinkedList(list);
  return 0;
}
