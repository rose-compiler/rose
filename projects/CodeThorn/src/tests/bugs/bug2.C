template <typename T, typename R>
class BoundFunctorCallbackImpl {
public:
  template <typename FUNCTOR>
  R operator() (void) {
    return m_functor (m_a);
  }
private:
  T m_functor;
  T m_a;
};
