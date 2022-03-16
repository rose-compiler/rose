#ifndef GeneralPriorityWorkList_H
#define GeneralPriorityWorkList_H

#include <sstream>
#include <vector>
#include <cstddef>
#include <queue>

#include "GeneralWorkList.h"
#include "TopologicalSort.h"

namespace CodeThorn {

  template<typename Element> struct GeneralPriorityElement {
  GeneralPriorityElement(int priority, Element data):priority(priority),data(data){}
    std::string toString() const {
      std::stringstream ss;
      ss<<"(pri:"<<priority<<",data:"<<data.toString()<<")";
      return ss.str();
    }
    int priority;
    Element data;
    ~GeneralPriorityElement() {
      if(std::is_pointer<Element>::value) {
        //cout<<"is pointer"<<endl;
      }
    }
  };
  template<typename Element>
  bool operator<(const GeneralPriorityElement<Element>& e1, const GeneralPriorityElement<Element>& e2);
  template<typename Element>
  bool operator==(const GeneralPriorityElement<Element>& e1, const GeneralPriorityElement<Element>& e2);
  template<typename Element>
  bool operator!=(const GeneralPriorityElement<Element>& c1, const GeneralPriorityElement<Element>& c2);
  
  // functor object to provide greater operator as predicate
  template<typename Element>
  struct GeneralPriorityElementGreaterOp {
    bool operator()(const GeneralPriorityElement<Element>& e1, const GeneralPriorityElement<Element>& e2) {
      return !(e1==e2||e1<e2);
    }
  };
  template<typename Element>
  struct GeneralPriorityElementSmallerOp {
    bool operator()(const GeneralPriorityElement<Element>& e1, const GeneralPriorityElement<Element>& e2) {
      return e1<e2;
    }
  };

  template <typename Element>
class GeneralPriorityWorkList {
  public:
    GeneralPriorityWorkList(CodeThorn::TopologicalSort::LabelToPriorityMap map);
    bool empty();
    void push(Element el);
    Element top();
    void pop();

    // overriding non-priority work list
    void push_front(Element el);
    void pop_front();
    Element front();
    void push_back(Element);

    std::size_t size();
    void clear();
    typedef GeneralPriorityElement<Element> GeneralPriorityElementType;
    // min priority queue (smaller elements are orderered first)
    //typedef std::priority_queue <GeneralPriorityElementType,std::vector<GeneralPriorityElementType>,GeneralPriorityElementGreaterOp<Element> > ElementMinPriorityQueueType;
    // use an ordered set to get a unique elements priority queue
    typedef std::set <GeneralPriorityElementType,GeneralPriorityElementSmallerOp<Element> > ElementMinPriorityQueueType;
    //typedef std::priority_queue <GeneralPriorityElementType> ElementMaxPriorityQueueType;
    typedef ElementMinPriorityQueueType ElementPriorityQueueType;
    void print();
 protected:
    TopologicalSort::LabelToPriorityMap _labelToPriorityMap;
    ElementPriorityQueueType _list;
  private:
  };
}
#include "GeneralPriorityWorkList.C"

#endif
