enum violation{ E1=0, E2, E3 }; // This is a violation

class Class
{
  enum classEnum{ E1=1, E2, E3 };

  public:
    Class(){}
    ~Class(){}
}; //class Class

namespace Namespace
{
  enum namespaceEnum{ E1=2, E2, E3 };
} //namespace Namespace
