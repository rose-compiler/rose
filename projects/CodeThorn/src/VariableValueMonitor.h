#ifndef VARIABLE_VALUE_MONITOR
#define VARIABLE_VALUE_MONITOR

#include "VariableIdMapping.h"

namespace CodeThorn {

  class Analyzer;
  class EState;
  class PState;

  class VariableValueMonitor {
  public:
    enum VariableMode { VARMODE_FORCED_TOP, VARMODE_ADAPTIVE_TOP, VARMODE_PRECISE, VARMODE_FORCED_PRECISE};
    VariableValueMonitor();
    void setThreshold(size_t threshold);
    size_t getThreshold();
    bool isActive();
    // the init function only uses the variableIds of a given estate (not its values) for initialization
    void init(const EState* estate);
    void init(const PState* pstate);
    SPRAY::VariableIdSet getHotVariables(Analyzer* analyzer, const EState* estate);
    SPRAY::VariableIdSet getHotVariables(Analyzer* analyzer, const PState* pstate);
    SPRAY::VariableIdSet getVariables();
    void setVariableMode(VariableMode,SPRAY::VariableId);
    VariableMode getVariableMode(SPRAY::VariableId);
    void update(Analyzer* analyzer, EState* estate);
    bool isHotVariable(Analyzer* analyzer, SPRAY::VariableId varId);
    std::string toString(SPRAY::VariableIdMapping* variableIdMapping);
#if 0
    bool isVariableBeyondTreshold(Analyzer* analyzer, SPRAY::VariableId varId);
#endif
  private:
    std::map<SPRAY::VariableId,std::set<int>* > _variablesMap;
    std::map<SPRAY::VariableId,VariableMode> _variablesModeMap;
    long int _threshold;
};

} // end of namespace CodeThorn

#endif
