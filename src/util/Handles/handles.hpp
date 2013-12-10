
#ifndef __HANDLES_HANDLES_HPP__
#define __HANDLES_HANDLES_HPP__

namespace Handles {

template <class Handler_tpl, typename operation_tpl = typename Handler_tpl::default_operation_t>
struct handle_t {
  typedef Handler_tpl Handler;

  typedef operation_tpl operation_t;
  typedef typename operation_t::fields_t fields_t;
  typedef typename operation_t::opcode_e opcode_e;

  opcode_e operation;
  fields_t fields;
};

template <class handle_tpl>
struct return_handle_t {
  typedef handle_tpl handle_t;

  typedef typename handle_t::operation_t::return_fields_t fields_t;

  fields_t fields;
};

}

#endif /* __HANDLES_HANDLES_HPP__ */

