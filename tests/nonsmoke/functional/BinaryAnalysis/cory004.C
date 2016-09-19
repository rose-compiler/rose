#include <rose.h>
#include <DispatcherX86.h>
#include <SymbolicSemantics2.h>

using namespace rose::BinaryAnalysis::InstructionSemantics2;

typedef SymbolicSemantics::SValue SymbolicValue;
typedef SymbolicSemantics::SValuePtr SymbolicValuePtr;
typedef DispatcherX86 RoseDispatcherX86;

typedef boost::shared_ptr<class SymbolicRegisterState> SymbolicRegisterStatePtr;

class SymbolicRegisterState: public BaseSemantics::RegisterStateGeneric {
protected:
    explicit SymbolicRegisterState(const SymbolicValuePtr &proto, const RegisterDictionary *rd):
        BaseSemantics::RegisterStateGeneric(proto, rd) {
        myclear();
    }

public:
    static SymbolicRegisterStatePtr instance(const SymbolicValuePtr &proto, const RegisterDictionary *rd) {
        return SymbolicRegisterStatePtr(new SymbolicRegisterState(proto, rd));
    }

    void myclear() {
        DispatcherX86Ptr dispatcher = RoseDispatcherX86::instance();
        dispatcher->set_register_dictionary(regdict);
        initialize_nonoverlapping(dispatcher->get_usual_registers(), false);
    }
};

int
main() {
    SymbolicValuePtr svalue = SymbolicValue::instance();
    const RegisterDictionary* regdict = RegisterDictionary::dictionary_pentium4();
    SymbolicRegisterStatePtr regs = SymbolicRegisterState::instance(svalue, regdict);
    std::cout <<"Initialized registers:\n" <<*regs;
}

