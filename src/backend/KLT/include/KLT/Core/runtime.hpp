
#ifndef __KLT_CORE_RUNTIME_HPP__
#define __KLT_CORE_RUNTIME_HPP__

namespace KLT {

template <class Annotation, class Language, class Runtime>
class Kernel;

namespace Runtime {

template <class Annotation, class Language, class RT>
void get_exec_config(typename RT::exec_config_t & exec_config, const Kernel<Annotation, Language, RT> * kernel) {}

template <class RT>
bool is_exec_mode(const typename RT::exec_mode_t & modes, size_t mode) {
  assert(mode < RT::num_exec_modes);
  return (modes & (1 << mode)) == (1 << mode);
}

template <class RT>
bool are_both_exec_mode(const typename RT::exec_mode_t & modes_1, const typename RT::exec_mode_t & modes_2, size_t mode) {
  assert(mode < RT::num_exec_modes);
  return (modes_1 & (1 << mode)) == (modes_2 & (1 << mode));
}

template <class RT>
void set_exec_mode(typename RT::exec_mode_t & modes, size_t mode) {
  assert(mode < RT::num_exec_modes);
  modes |= (1 << mode);
}

template <class RT>
void unset_exec_mode(typename RT::exec_mode_t & modes, size_t mode) {
  assert(mode < RT::num_exec_modes);
  modes &= ~(typename RT::exec_mode_t)(1 << mode);
}

}

}

#endif /* __KLT_CORE_RUNTIME_HPP__ */


