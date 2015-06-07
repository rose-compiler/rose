namespace ns {

class Member {

public:
  void member_function()
  { }
};

template <typename T>
class TemplateMember {

public:

  TemplateMember();

  void member_function();
};

template<typename T>
TemplateMember<T>::TemplateMember()
{ }

template<typename T>
void TemplateMember<T>::member_function()
{ }

template<typename T>
class TC {
public:
  TC(void(*func) (void));

  TC(void(Member::*func) (void));

  TC(void(TemplateMember<int>::*func) (void));
};

template<typename T>
TC<T>::TC(void(*func) (void))
{ }

template<typename T>
TC<T>::TC(void(Member::*func) (void))
{ }

template<typename T>
TC<T>::TC(void(TemplateMember<int>::*func) (void))
{ }

void func()
{ }

} // end of namespace ns

int main()
   {
  // Breaks only when classes are in namespace
     ns::TC<int> t(&ns::func); // works
     ns::TC<int> *t1 = new ns::TC<int>(&ns::func); // works

  // Original code:
  // ns::TC<int> *t2 = new ns::TC<int>(&ns::Member::member_function); // breaks
     ns::TC<int> *t2 = new ns::TC<int>(&ns::Member::member_function); // breaks

  // Original code:
  // ns::TC<int> *t3 = new ns::TC<int>(&ns::TemplateMember<int>::member_function); // breaks
     ns::TC<int> *t3 = new ns::TC<int>(&ns::TemplateMember<int>::member_function); // breaks

     return 0;
   }
