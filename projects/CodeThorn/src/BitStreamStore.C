#include "BitStreamStore.h"

BitStreamStore::BitStreamStore():_pos(0) {
}

void BitStreamStore::write(int value, int numBits) {
  for(int i=numBits-1;i>=0;--i) {
	bool isSetBit=isSet(i,value);
	_bits.push_back(isSetBit);
  }
}

int BitStreamStore::lastPos() {
  return _bits.size()-1;
}

int BitStreamStore::readBack(int numBits, int& pos) {
  int value=0;
  for(int i=0;i<numBits;i++) {
	bool bit=_bits[pos--];
	if(bit)
	  set(i,value);
  }
  return value;
}

int BitStreamStore::readFront(int numBits, int& pos) {
  int value=0;
  for(int i=numBits-1;i>=0;i--) {
	bool bit=_bits[pos++];
	if(bit)
	  set(i,value);
  }
  return value;
}

string BitStreamStore::toString() {
  string s;
  for(int i=0;i<_bits.size();i++) {
	if(_bits[i]==true)
	  s+="1";
	else
	  s+="0";
  }
  return s;
}

bool BitStreamStore::isSet(short bitPos,int& value) {
  int mask=(1<<bitPos);
  return ((value&mask)>>bitPos)==1;
}

void BitStreamStore::set(short bitPos,int& value) {
  int mask=(1<<bitPos);
  value=value|mask;
}
