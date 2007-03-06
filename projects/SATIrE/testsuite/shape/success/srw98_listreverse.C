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

L* reverseList(L* x) {
	L* y;
	L* t;
	y=0;
	while(x!=0) {
		t=y;		
		y=x;		
		x=x->next;
		y->next=t;
	}
	t=0;
	return y;
}

int main(){
	L* acyclicList;
	L* reversedList;
	acyclicList=createList(10);
	reversedList=reverseList(acyclicList);
	acyclicList=0;
 
	return reversedList==0;
}
