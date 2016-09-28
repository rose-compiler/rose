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
