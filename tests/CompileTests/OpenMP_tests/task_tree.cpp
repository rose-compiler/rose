/*
 * OMP 3.0 p.178
 * Example A.13.1c
 *
 * OpenMP tasks will be executed asynchronously 
 * Assuming pre-order/post-order of traversal is wrong
 * unless #pragma omp taskwait is used in the end
 * 9/12/2008
 */

#include<iostream> 

// DQ (9/12/2009): GNU g++ 4.4 requires stdio.h so that printf() will be defined.
// Note that this is not required for GNU g++ version 4.3.
#include <stdio.h>

#ifdef _OPENMP
#include<omp.h>
#endif

using namespace std;

//A simple binary tree node
class node {
  public:
    int id;
    node* parent;
    class node* left; // class node or node here? Both
    node* right;
    node(int id):id(id){}
    void setLeft(node* child) { left = child; child->parent = this;}
    void setRight(node* child){ right = child; child->parent = this;}
};

void process(node* n) 
{
#pragma omp critical  
  printf("Found node %d\n",n->id);
}

void traverse (struct node *p)
{
  //process(p); // pre-order
  if (p->left)
  {
#pragma omp task
    traverse(p->left);
  }
 // else // very wrong!!
  
  if(p->right) 
  {
#pragma omp task
     traverse(p->right);
  }
//#pragma omp taskwait     
  process(p); // post-order   
}


struct node * tree;
/*!
 *      1
 *     /  \
 *    2    3
 *   /  \  / \
 *  4   5  6  7
 * /\   /\
 * 8 9 10
 *
 */
void init()
{
  tree = new node(1);
  node* child2 = new node(2);  
  node* child3 = new node(3);  
  node* child4 = new node(4);  
  node* child5 = new node(5);  
  node* child6 = new node(6);  
  node* child7 = new node(7);  
  node* child8 = new node(8);  
  node* child9 = new node(9);  
  node* child10 = new node(10);  

  tree->setLeft(child2);
  tree->setRight(child3);

  child2->setLeft(child4);
  child2->setRight(child5);
  
  child3->setLeft(child6);
  child3->setRight(child7);

  child4->setLeft(child8);
  child4->setRight(child9);

  child5->setLeft(child10);
}

int main()
{
  init(); 
#ifdef _OPENMP  
  omp_set_num_threads(4);
#endif  

//// wrong? seg fault
//#pragma omp parallel  

//#pragma omp parallel sections
#pragma omp parallel 
{
#pragma omp single
  traverse(tree);
}
  return 0;
}

