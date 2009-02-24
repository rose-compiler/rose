#ifndef PROGRAM_VERIFICATION_H
#define PROGRAM_VERIFICATION_H



class Conditions;

class ProgramVerifier
{
public:
    bool verify(SgFunctionDeclaration *, Conditions *);
    std::string report();
};

Conditions *classInvariants(SgClassDefinition *);
Conditions *functionPrePostConditions(SgFunctionDeclaration *);
Conditions *combineConditions(Conditions *, Conditions *);

#endif
