#include "srw98_data.h"


L* createList(int n){
	L* listHead=0;
	L* newElement;
	
	while(n>0) {
		newElement = new L;
		newElement->next = listHead;
		listHead = newElement;
		n--;
	}
		return listHead;
}

void insertList(L* destination, L* source, int pos) {
	L* dest;
	L* src;
	L* srcLast;

	dest = destination;
	src = source;

	srcLast = src;
	while(srcLast->next!=0){
		srcLast = srcLast->next;
	}

	while((dest->next != 0) && (pos !=0)) {
		dest = dest->next;
		pos--;
	}
	
	srcLast->next = dest->next;
	dest->next=src;

	dest=0;
	src=0;
	srcLast=0;
}

int main(){
	L* list1;
	L* list2;

	list1 = createList(5);
	list2 = createList(3);

	insertList(list1,list2,3);

	return 0;
}
