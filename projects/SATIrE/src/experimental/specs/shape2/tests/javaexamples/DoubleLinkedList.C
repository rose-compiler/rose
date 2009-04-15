#define NULL ((DoubleLinkedList*)0)
using namespace std;

// implementation of an acyclic double-linked and sorted list
class DoubleLinkedList {
  public:
    int element;
    class DoubleLinkedList *next;
    class DoubleLinkedList *forward;
};

// shows the elements of the list
void ShowList(DoubleLinkedList *list) {
	DoubleLinkedList *l = list;
	while (l != NULL) {
	  // << l->element << endl;
	  l = l->next;
	}
}

// creates a new list of class DoubleLinkedList and returns the 
// created list of a given length
DoubleLinkedList *CreateList(int length) {
	int i = length;
	DoubleLinkedList *list = NULL;
	DoubleLinkedList *dummylist = NULL;

	while (i > 0) {
	  list = new DoubleLinkedList();
	  list->element = i;
	  list->next = dummylist;
	  list->forward = NULL;
	  dummylist = list;
	  i--;
	}
	return (list);
}

// inserts a new element in the existing list      
DoubleLinkedList *InsertInList(DoubleLinkedList *list, DoubleLinkedList *e) {
	DoubleLinkedList *y = list;
	if (y->element < e->element) {
	  while (y->element < e->element && y->next != NULL)	{
		  y = y->next;
	  }
	  e->next = y->next;
	  e->forward = y;
	  y->next = e;
	}// if (y->element < e->element)
	else {
	  e->next = list;
	  e->forward = NULL;
	  list = e;
	}
	return list;
}

// removes an element of an DoubleLinkedList 
DoubleLinkedList *RemoveFromList(DoubleLinkedList *list, DoubleLinkedList *e) {
	DoubleLinkedList *l = list;
	DoubleLinkedList *dummy;

	if (l != NULL) {
	  if (l->element != e->element) {
		  while (l->next != NULL && e->element != l->next->element) {
		    l = l->next;
		  }
		  if (l->next != NULL) {
		    if (l->next->element == e->element) {
			    if (l->next->next == NULL)
			      l->next = NULL;  
			    else {
			      l->next = l->next->next;
		        l->next->forward = l;
			    }
		    }
		  } else {
		    // << "element not found in list" << endl;
      }
    } else {
		  list = list->next;
		  list->forward = NULL;
    }
	}	   
	else {
	  // << "list is empty" << endl;
	}
	return list;
}

// to show the results
int main(int argc, char **argv) {

  DoubleLinkedList *insertelement = new DoubleLinkedList();
  insertelement->element = 0;
  insertelement->next = NULL;
  insertelement->forward = NULL;

  DoubleLinkedList *removeelement = new DoubleLinkedList();
  removeelement->element = 1;
  removeelement->next = NULL;
  removeelement->forward = NULL;

  DoubleLinkedList *list = CreateList(6);
  // << "CreateList" << endl;
  ShowList(list);
  // << endl;
  // << endl;

  list = InsertInList(list, insertelement);
  // << "InsertInList: " << insertelement->element << endl;
  ShowList(list);
  // << endl;
  // << endl;

  list = RemoveFromList(list, removeelement);
  // << endl;
  // << endl;
  // << "RemoveFromList: " << removeelement->element << endl;
  ShowList(list);

  return 0;
}
