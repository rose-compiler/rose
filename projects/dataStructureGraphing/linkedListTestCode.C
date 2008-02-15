
#define NULL 0
#include<assert.h>

class listElement
   {
      public:
         int extraIntergerData;
         int extraFloatData;
         listElement* next;

         listElement() :extraIntergerData(42),extraFloatData(3.14),next(NULL) {}
         listElement* getNextElement();         
   };

class treeElement
   {
      public:
         int extraIntergerData;
         int extraFloatData;
         treeElement* leftChild;
         treeElement* rightChild;

         treeElement() :extraIntergerData(42),extraFloatData(3.14),leftChild(NULL),rightChild(NULL) {}
         treeElement* getNextElement();         
   };



int main()
   {
     listElement* previousElement = NULL;
     listElement* headOfList = new listElement();
     previousElement = headOfList;
     for (int i=0; i < 3; i++)
        {
          previousElement->next = new listElement();
          assert(previousElement->next != NULL);
          
          previousElement = previousElement->next;
        }

     treeElement* previousTreeElement = NULL;
     treeElement* headOfTree = new treeElement();
     previousElement = headOfList;
     for (int i=0; i < 3; i++)
        {
          previousTreeElement->leftChild  = new treeElement();
          previousTreeElement->rightChild = new treeElement();
          assert(previousTreeElement->leftChild != NULL);
          assert(previousTreeElement->leftChild != NULL);
          
          previousTreeElement = previousTreeElement->leftChild;
        }

  // PRINT THE LIST HERE 
   
     return 0;
   }
