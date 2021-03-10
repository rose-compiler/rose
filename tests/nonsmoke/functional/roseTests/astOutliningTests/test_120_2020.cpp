// Test the handling of private typedef types
// global B
class A
{
  public:
    void foobar();
  private:
    typedef int BListIter;
};

void A::foobar()
{
  BListIter bListIter;
  bool abvar;
#pragma rose_outline
  abvar = (bListIter != bListIter);
}
