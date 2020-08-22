#include "EStateWorkList.h"

size_t CodeThorn::EStateWorkList::size() {
  return _list.size();
}

bool CodeThorn::EStateWorkList::empty() {
  return _list.empty();
}

void CodeThorn::EStateWorkList::clear() {
  return _list.clear();
}

void CodeThorn::EStateWorkList::push_front(const EState* el) {
  _list.push_front(el);
}

const CodeThorn::EState* CodeThorn::EStateWorkList::front() {
  return _list.front();
}

void CodeThorn::EStateWorkList::pop_front() {
  _list.pop_front();
}

void CodeThorn::EStateWorkList::push_back(const EState* el) {
  _list.push_back(el);
}

CodeThorn::EStateWorkList::iterator CodeThorn::EStateWorkList::begin() {
  return _list.begin();
}

CodeThorn::EStateWorkList::iterator CodeThorn::EStateWorkList::end() {
  return _list.end();
}
