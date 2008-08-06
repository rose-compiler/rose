#include <string>
class MixMatmodel{

  public:
    double fieldReal(std::string str){ return 1; };

};

int main(){
    MixMatmodel x;

    //this is a rule violation
    int y     = x.fieldReal("test1");
    //this is conformant to the rule
    int test1 = x.fieldReal("test1");

    //this is conformant to the rule
    int test2 = x.fieldReal("test1:test2");
    //this is a violation of the rule
    test2 = x.fieldReal("test1:test2:test3");

};
