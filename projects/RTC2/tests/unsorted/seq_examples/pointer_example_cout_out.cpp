
struct __Pb__L404R__Pe___Type 
{
  std::basic_ostream< char ,std::char_traits< char  > > ::__streambuf_type *ptr;
  unsigned long long addr;
}
;

struct __Pb___Fb___Rb__L307R__Re___Gb___Rb__L307R__Re___Fe___Pe___Type 
{
  __ostream_type &(*ptr)(__ostream_type &);
  unsigned long long addr;
}
;

struct __Pb___Fb___Rb__L302R__Re___Gb___Rb__L302R__Re___Fe___Pe___Type 
{
  __ios_type &(*ptr)(__ios_type &);
  unsigned long long addr;
}
;

struct __Pb___Fb___Rb__std__scope__ios_base__Re___Gb___Rb__std__scope__ios_base__Re___Fe___Pe___Type 
{
  class ios_base &(*ptr)(class ios_base &);
  unsigned long long addr;
}
;

struct __Pb__Cv__Pe___Type 
{
  const void *ptr;
  unsigned long long addr;
}
;

struct __Pb__CL274R__Pe___Type 
{
  const std::char_traits< char > ::char_type *ptr;
  unsigned long long addr;
}
;
#include <iostream.h>
#include <cstdlib>

struct __Pb__L414R__Pe___Type 
{
  std::ostream *ptr;
  unsigned long long addr;
}
;
static struct __Pb__L414R__Pe___Type __Pb__L414R__Pe___Type_Ret_AddressOf_UL_Arg_Ui_Arg(unsigned long long ,unsigned int );

static struct __Pb__L414R__Pe___Type __Pb__L414R__Pe___Type_Ret_AddressOf_UL_Arg_Ui_Arg(unsigned long long input1,unsigned int input2)
{
  struct __Pb__L414R__Pe___Type output;
  output.__Pb__L414R__Pe___Type::ptr = ((std::ostream *)input1);
  output.__Pb__L414R__Pe___Type::addr = ((unsigned long long )(&output.__Pb__L414R__Pe___Type::ptr));
  v_Ret_create_entry_UL_Arg_UL_Arg_Ul_Arg(output.__Pb__L414R__Pe___Type::addr,((unsigned long long )output.__Pb__L414R__Pe___Type::ptr),((unsigned long )input2));
  return output;
}
void v_Ret_create_entry_UL_Arg_UL_Arg_Ul_Arg(unsigned long long ,unsigned long long ,unsigned long );

struct __Pb__L306R__Pe___Type 
{
  class std::basic_ostream< char  , std::char_traits< char  >  > *ptr;
  unsigned long long addr;
}
;
static struct __Pb__L306R__Pe___Type __Pb__L306R__Pe___Type_Ret_Cast___Pb__L414R__Pe___Type_Arg(struct __Pb__L414R__Pe___Type );

static struct __Pb__L306R__Pe___Type __Pb__L306R__Pe___Type_Ret_Cast___Pb__L414R__Pe___Type_Arg(struct __Pb__L414R__Pe___Type input1)
{
  struct __Pb__L306R__Pe___Type output;
  output.__Pb__L306R__Pe___Type::ptr = ((class std::basic_ostream< char  , std::char_traits< char  >  > *)input1.__Pb__L414R__Pe___Type::ptr);
  output.__Pb__L306R__Pe___Type::addr = input1.__Pb__L414R__Pe___Type::addr;
  return output;
}
static class std::basic_ostream< char  , std::char_traits< char  >  > *__Pb__L306R__Pe___Ret_Deref___Pb__L306R__Pe___Type_Arg(struct __Pb__L306R__Pe___Type );

static class std::basic_ostream< char  , std::char_traits< char  >  > *__Pb__L306R__Pe___Ret_Deref___Pb__L306R__Pe___Type_Arg(struct __Pb__L306R__Pe___Type input1)
{
  v_Ret_check_entry_UL_Arg_UL_Arg(((unsigned long long )input1.__Pb__L306R__Pe___Type::ptr),input1.__Pb__L306R__Pe___Type::addr);
  return input1.__Pb__L306R__Pe___Type::ptr;
}
void v_Ret_check_entry_UL_Arg_UL_Arg(unsigned long long ,unsigned long long );

int main()
{
  ( *__Pb__L306R__Pe___Ret_Deref___Pb__L306R__Pe___Type_Arg(__Pb__L306R__Pe___Type_Ret_Cast___Pb__L414R__Pe___Type_Arg(__Pb__L414R__Pe___Type_Ret_AddressOf_UL_Arg_Ui_Arg(((unsigned long long )(&std::cout)),sizeof(std::cout))))<<"Test message") << std::endl< char  , std::char_traits< char  >  > ;
  return 0;
}

struct __Pb__L274R__Pe___Type 
{
  std::char_traits< char > ::char_type *ptr;
  unsigned long long addr;
}
;
