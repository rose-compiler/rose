#include "sage3basic.h"
#include "GeneralWorkList.h"

template<typename Element>
CodeThorn::GeneralWorkList<Element>::~GeneralWorkList() {
}

template<typename Element>
std::size_t CodeThorn::GeneralWorkList<Element>::size() {
  return _list.size();
}

template<typename Element>
bool CodeThorn::GeneralWorkList<Element>::empty() {
  return _list.empty();
}

template<typename Element>
void CodeThorn::GeneralWorkList<Element>::clear() {
  return _list.clear();
}

template<typename Element>
void CodeThorn::GeneralWorkList<Element>::push_front(Element el) {
  _list.push_front(el);
}

template<typename Element>
Element CodeThorn::GeneralWorkList<Element>::front() {
  return _list.front();
}

template<typename Element>
void CodeThorn::GeneralWorkList<Element>::pop_front() {
  _list.pop_front();
}

template<typename Element>
void CodeThorn::GeneralWorkList<Element>::push_back(Element el) {
  _list.push_back(el);
}

template<typename Element>
auto CodeThorn::GeneralWorkList<Element>::begin() const->decltype(_list.begin()) {
  return _list.begin();
}

template<typename Element>
auto CodeThorn::GeneralWorkList<Element>::end()  const ->decltype(_list.end()) {
  return _list.end();
}
