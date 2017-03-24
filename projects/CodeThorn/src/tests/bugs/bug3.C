template <typename T, typename R>
class BoundFunctorCallbackImpl {
public:
  template <typename FUNCTOR, typename ARG>
  BoundFunctorCallbackImpl (FUNCTOR functor, ARG a)
    : m_functor (functor), m_a (a) {}

private:
  T m_functor;
  T m_a;
};
