
#ifndef __HANDLES_HOOKS_HPP__
#define __HANDLES_HOOKS_HPP__

namespace Handles {

template <class Handler_tpl, typename operation_tpl> struct handle_t;
template <typename handle_tpl> struct return_handle_t;

struct hook_t {};

template <class Handler_tpl, typename operation_tpl>
struct handle_hook_t : public virtual ::Handles::hook_t {
  typedef Handler_tpl Handler;
  typedef operation_tpl operation_t;

  typedef typename ::Handles::handle_t<Handler, operation_t> handle_t;
  typedef ::Handles::return_handle_t<handle_t> return_handle_t;

  const unsigned long submit(handle_t *);
  return_handle_t * get_result(const unsigned long);
}; 

}

#endif /* __HANDLES_HOOKS_HPP__ */

