/*
 * The single thread will join to do the processing?
 * The other threads can do the processing before 
 * the single thread finishes generating tasks?
 * 
 * Based on OMP spec. 3.0 Example A.13.3c
 * By Liao
 * 9/12/2008
 */

#include <iostream>
#include <omp.h>
using namespace std;
class node
{
public:  
  int data;
  node *next;
  node(int i):data(i){}
  void setNext(node* n){next = n;}
};

void process (node * p)
{
  cout<<"Found node:"<<p->data;
#if _OPENMP  
  cout<<" by thread:"<<omp_get_thread_num();
#endif  
  cout<<endl;
}

node* root;

void init()
{
  root = new node(1);
  node* node2= new node(2);
  root->setNext(node2);
  node* node3= new node(3);
  node2->setNext(node3);
  node* node4 = new node(4);
  node3->setNext(node4);
  node* node5 = new node(5);
  node4->setNext(node5);

  node* node6= new node(6);
  node5->setNext(node6);
  node* node7= new node(7);
  node6->setNext(node7);
  node* node8= new node(8);
  node7->setNext(node8);
  node* node9= new node(9);
  node8->setNext(node9);
  node* node10= new node(10);
  node9->setNext(node10);
}
void
increment_list_items (node * head)
{
#pragma omp parallel
  {
#pragma omp single
    {
#if _OPENMP      
      cout<<"Total threads :"<<omp_get_num_threads();
      cout<<", reported by thread:"<<omp_get_thread_num()<<endl;
#endif      
      node *p = head;
      while (p)
	{
// p is firstprivate by default
//#pragma omp task if (0)
#pragma omp task 
	  process (p);
//#pragma omp taskwait          
	  p = p->next;
	} // end while
    } // end single
  }// end parallel
}

int main()
{
  init();
  increment_list_items(root);
  return 0;
}

