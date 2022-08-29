#include "conditionalDisable.h"
#ifdef ROSE_BINARY_TEST_DISABLED
#include <iostream>
int main() { std::cout <<"disabled for " <<ROSE_BINARY_TEST_DISABLED <<"\n"; return 1; }
#else

#include <rose.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherX86.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/SymbolicSemantics.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>

using namespace Rose::BinaryAnalysis;
using namespace Rose::BinaryAnalysis::InstructionSemantics;

typedef SymbolicSemantics::SValue SymbolicValue;
typedef SymbolicSemantics::SValuePtr SymbolicValuePtr;
typedef DispatcherX86 RoseDispatcherX86;

typedef boost::shared_ptr<class SymbolicRegisterState> SymbolicRegisterStatePtr;

class SymbolicRegisterState: public BaseSemantics::RegisterStateGeneric {
protected:
    explicit SymbolicRegisterState(const SymbolicValuePtr &proto, const RegisterDictionary::Ptr &rd):
        BaseSemantics::RegisterStateGeneric(proto, rd) {
        myclear();
    }

public:
    static SymbolicRegisterStatePtr instance(const SymbolicValuePtr &proto, const RegisterDictionary::Ptr &rd) {
        return SymbolicRegisterStatePtr(new SymbolicRegisterState(proto, rd));
    }

    void myclear() {
        DispatcherX86Ptr dispatcher = RoseDispatcherX86::instance();
        dispatcher->registerDictionary(regdict);
        initialize_nonoverlapping(dispatcher->get_usual_registers(), false);
    }
};

int
main() {
    SymbolicValuePtr svalue = SymbolicValue::instance();
    RegisterDictionary::Ptr regdict = RegisterDictionary::instancePentium4();
    SymbolicRegisterStatePtr regs = SymbolicRegisterState::instance(svalue, regdict);
    std::cout <<"Initialized registers:\n" <<*regs;
}

#endif
