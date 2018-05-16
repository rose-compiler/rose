template <typename t_Type1, typename t_Type2>
class map {
};

struct string {
};

namespace namespace1 {

  template <typename t_t_parm1>
  using t_Class1 =
  map<string, string>;

  class Class2 {
  public:
    virtual t_Class1<double> func1() const = 0;
  };
}

namespace1::t_Class1<double> var1;
namespace1::t_Class1<double> var2;


#if 0
// generates this:
template < typename t_Type1, typename t_Type2 >
class map {
};

struct string
{
}
;
namespace namespace1
{
template < typename t_t_parm1 >
  using t_Class1 =
  map < string, string >;

class Class2
{
public: virtual t_Class1 < double  >  func1() const = 0;
}
;
}
t_Class1 < double  >  var1;
namespace1::Class2::t_Class1 < double  >  var2;

// which gets these errors:
rose_ROSE-26.cc(21): error: t_Class1 is not a template
  t_Class1 < double  >  var1;
  ^

rose_ROSE-26.cc(22): error: class "namespace1::Class2" has no member "t_Class1"
  namespace1::Class2::t_Class1 < double  >  var2;
                     ^

rose_ROSE-26.cc(22): error: expected a ";"
  namespace1::Class2::t_Class1 < double  >  var2;
                     ^
#endif
