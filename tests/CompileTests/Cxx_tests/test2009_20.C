// Liao, 5/23/2009
// Test calling base class's constructor within a constructor initializer list
// parenthesis must be preserved!
// A test case extracted from spec cpu2006's 450.soplex
class BASE
{
  //No user-defined constructor
  public:
  int i;
//  BASE():i(0){};
};

class CHILD :public BASE
{
public:
  int j;
 // copy constructor
  explicit CHILD(const CHILD& old);

};
// Call a compiler-generated base class's constructor
// () must be preserved
CHILD::CHILD(const CHILD& old):BASE()
{
  j=old.j;
}

