
#ifndef __KLT_CORE_CG_CONFIG_HPP__
#define __KLT_CORE_CG_CONFIG_HPP__

#include <cstddef>

namespace KLT {

namespace Core {

class DataFlow;
class LoopSelector;

class CG_Config {
  protected:
    DataFlow     * p_data_flow;
    LoopSelector * p_loop_selector;

  public:
    CG_Config(DataFlow * data_flow, LoopSelector * loop_selector);
    virtual ~CG_Config();

    const DataFlow & getDataFlow() const;
    const LoopSelector & getLoopSelector() const;
};

}

}

#endif /* __KLT_CORE_CG_CONFIG_HPP__ */

