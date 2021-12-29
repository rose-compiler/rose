#include "sage3basic.h"
#include "EStateWorkList.h"

CodeThorn::EStateWorkList::~EStateWorkList() {
}

std::size_t CodeThorn::EStateWorkList::size() {
  return _list.size();
}

bool CodeThorn::EStateWorkList::empty() {
  return _list.empty();
}

void CodeThorn::EStateWorkList::clear() {
  return _list.clear();
}

void CodeThorn::EStateWorkList::push_front(EStatePtr el) {
  _list.push_front(el);
}

EStatePtr CodeThorn::EStateWorkList::front() {
  return _list.front();
}

void CodeThorn::EStateWorkList::pop_front() {
  _list.pop_front();
}

void CodeThorn::EStateWorkList::push_back(EStatePtr el) {
  _list.push_back(el);
}

