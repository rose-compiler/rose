#ifndef BITSTREAMSTORE
#define BITSTREAMSTORE

#include <vector>
#include <string>
#include <iostream>

using namespace std;

class BitStreamStore {
 public:
  typedef unsigned short internalType;
  BitStreamStore();
  void write(int value, int numBits);
  int lastPos();
  // read 'numBits' bits. Pass the position to read from in 'pos'. 'pos' is updated to new position.
  // the function returns the int value which was read from the 'numBits' bits.
  int readBack(int numBits, int& pos);
  int readFront(int numBits, int& pos);
  string toString();
 private:
  bool isSet(short bitPos,int& value);
  void set(short bitPos,int& value);
  vector<bool> _bits;
  short _pos;
};

#endif
