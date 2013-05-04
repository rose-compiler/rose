// We want to avoid calling critical sections from critical sections:
// therefore all worklist functions do not use each other.

#include <list>
using namespace std;
using namespace CodeThorn;

template<typename Element>
bool WorkList<Element>::isEmpty() { 
  bool res;
  #pragma omp critical
  {
	res=(workList.size()==0);
  }
  return res;
}

template<typename Element>
bool WorkList<Element>::exists(Element estate) {
  bool res=false;
#pragma omp critical
  {
	for(typename list<Element>::iterator i=workList.begin();i!=workList.end();++i) {
	  if(*i==estate) res=true;break;
	}
  }
  return res;
}

template<typename Element>
void WorkList<Element>::add(Element estate) { 
#pragma omp critical
  {
	workList.push_back(estate); 
  }
}

template<typename Element>
Element WorkList<Element>::take() {
  if(workList.size()==0)
	throw "Error: attempted to take element from empty work list.";
  Element co;
#pragma omp critical
  {
  if(workList.size()>0) {
	co=*workList.begin();
	workList.pop_front();
  }
  }
  return co;
}

template<typename Element>
Element WorkList<Element>::top() {
  Element estate=0;
#pragma omp critical
  {
	if(workList.size()>0)
	  estate=*workList.begin();
  }
  return estate;
}

template<typename Element>
Element WorkList<Element>::pop() {
  Element estate=0;
  #pragma omp critical
  {
	if(workList.size()>0)
	  estate=*workList.begin();
	if(estate)
	  workList.pop_front();
  }
  return estate;
}
