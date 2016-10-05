// This is a test submitted by Brian White (7/4/2007).
// There is an associated test code in:
//    ROSE/developersScratchSpace/Dan/translator_tests/brianWhite.C

class Init 
{
public:

  static bool
  _S_initialized() { return _S_ios_base_init; }

  static int _S_ios_base_init;  

};

int main()
{
  return 0;
}
