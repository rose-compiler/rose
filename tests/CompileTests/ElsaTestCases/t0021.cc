// cc.in21
// demonstrate variable vs. class namespace

class Gronk {
};

//typedef class Gronk Gronk;    // implicit

// this isn't an error..
//typedef class Whammy Gronk;    // implicit

int main()
{
  int Gronk;

  class Gronk *g;      // -> type
  return Gronk;        // -> int
}


typedef int x;

// it turns out this isn't actually an error in C++
typedef int x;

//ERROR(2): typedef double x;

