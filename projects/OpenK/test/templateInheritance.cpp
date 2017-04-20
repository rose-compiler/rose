//#include <vector>
#include <iostream>
#include <cassert>

using namespace std;

//----------base template class --------------
const int Size = 100;
template <typename Foo>
class QueueT 
{
  protected: // cannot be private since the derived class wants to access it
    Foo buffer[Size];
    int Head, Tail, Count;
  public:
    QueueT () {Head = 0; Tail = -1; Count = 0;};
    void Enqueue (Foo Item) 
    {
      if (!isFull())
      {
        Tail ++; 
        buffer[Tail] = Item; 
        Count ++; 
      }
      else
      {
        cerr<<"Cannot enqueue since the queue is already full."<<endl;
        assert (0);
      }
    };
    Foo Dequeue () 
    {
      if (Count>=1)
      {
        Tail --;
        Count --; 
        cout<<Tail+1<<endl;
        return buffer[Tail+1];
      }  
      else
      {
        cerr<<"Cannot dequeue since the queue is already empty."<<endl;
        assert (0);
      }
    };
    int getSize() const {return Count;}
    bool isFull() const {return (Count== 100);};
    ~QueueT() {};
};


template <typename Foo>  // another template argument
class InspectableQueueT : public QueueT <Foo>  // always provide the argument when using a template
{
  public:
   InspectableQueueT () { 

     } ;
   Foo Inspect() { 
     //return QueueT <Foo>::buffer[QueueT <Foo>::Tail]; // only works for public members
     return this->buffer[this->Tail]; // only works for public members
     } ; // unbuffer and Tail??
   ~InspectableQueueT () {};
};


int main()
{
  QueueT <int> integerQ;
  integerQ.Enqueue (17);
  cout<<"dequeue:"<<integerQ.Dequeue()<<endl;
  // Try the derived template class
  InspectableQueueT <float> floatQ;
  floatQ.Enqueue (9.909);
  cout<<"inspect:"<<floatQ.Inspect()<<endl;
  cout<<"inspect:"<<floatQ.Inspect()<<endl;
  return 0;
}
