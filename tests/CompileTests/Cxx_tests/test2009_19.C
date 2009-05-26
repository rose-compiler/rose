class BASE
{
  //No user-defined constructor
  public:
  int i;
  BASE():i(0){};
};

class CHILD :public BASE
{
public:
  int j;
 // copy constructor
  explicit CHILD(const CHILD& old);

};
// refer to compiler-generated base class's constructor
CHILD::CHILD(const CHILD& old):BASE()
{
  j=old.j;
}

