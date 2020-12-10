#include "sage3basic.h"
#include "ReadWriteHistory.h"
#include "CollectionOperators.h"
#include "EState.h"

using namespace std;
using namespace CodeThorn;
using namespace CodeThorn;

bool CodeThorn::operator<(const ReadsWritesUnorderedBlock& s1, const ReadsWritesUnorderedBlock& s2) {
  if (s1.previousReads != s2.previousReads)
    return s1.previousReads < s2.previousReads;
  if (s1.previousWrites != s2.previousWrites)
    return s1.previousWrites < s2.previousWrites;
  if (s1.currentReads != s2.currentReads)
    return s1.currentReads < s2.currentReads;
  return s1.currentWrites < s2.currentWrites;
}

bool CodeThorn::operator==(const ReadsWritesUnorderedBlock& s1, const ReadsWritesUnorderedBlock& s2) {
  return ( (s1.previousReads == s2.previousReads) &&
	   (s1.previousWrites == s2.previousWrites) &&
	   (s1.currentReads == s2.currentReads) &&
	   (s1.currentWrites == s2.currentWrites) );	   
}

bool CodeThorn::operator!=(const ReadsWritesUnorderedBlock& s1, const ReadsWritesUnorderedBlock& s2) {
  return !(s1==s2);
}


bool CodeThorn::operator<(const ReadWriteHistory& s1, const ReadWriteHistory& s2) {
  return (*(s1.stackOfUnorderedBlocks())<*(s2.stackOfUnorderedBlocks()));
}

bool CodeThorn::operator==(const ReadWriteHistory& s1, const ReadWriteHistory& s2) {
  return (*(s1.stackOfUnorderedBlocks())==*(s2.stackOfUnorderedBlocks()));
}

bool CodeThorn::operator!=(const ReadWriteHistory& s1, const ReadWriteHistory& s2) {
  return !(s1==s2);
}

string ReadWriteHistory::toString() const {
  stringstream ss;
  ss << "TODO: Implement function toString() in class ReadWriteHistory." << endl;
  return ss.str();
}

