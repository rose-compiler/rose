/*
Bug name:	functionCallExpressionPreceedsDeclarationWhichAssociatesScope

Reported by:	Brian White

Date:		Sept 28, 2007

Date sent:      Sept 30, 2007

E-mail subject: more SPEC 2006 bugs

Occurs in:  	SPEC CPU 2006 	471.omnetpp/src/EtherAppSrv.cc
                                471.omnetpp/src/LargeNet_n.cc
                                471.omnetpp/src/sim/util.cc
                                471.omnetpp/src/libs/sim/distrib.cc
                                and all of the many files that 
                                invoke Define_Module, Register_Class, 
                                Register_ModuleInterface, Define_Network,
                                Define_Link, Register_OmnetApp, Define_Function.

Brief description:  	I don't know what is happening here,
                        but manifesting the assertion failure
                        seems to require that a function pointer
                        be passed to doSomething.  The assertion
                        failure results from something not
                        yet implemented in 
                        functionCallExpressionPreceedsDeclarationWhichAssociatesScope.
                        The assertion failure occurs during unparsing
                        of the statement:
                        (cmod->doSomething(EtherAppSrv__create))->setOwner();

Files
-----
bug.cc		Trips the following assertion during _unparsing_ in the method
                functionCallExpressionPreceedsDeclarationWhichAssociatesScope

These are the acceptable cases, but not handled yet... 
rosec: /fusion/research4/bwhite/ROSE-0.8.10e/ROSE/src/frontend/SageIII/sageInterface/sageInterface.C:2380: SageInterface::functionCallExpressionPreceedsDeclarationWhichAssociatesScope(SgFunctionCallExp*)::DeclarationOrderTraversal::DeclarationOrderTraversal(SgFunctionCallExp*): Assertion `false' failed.

rose_bug.cc     The output from compiling bug.cc.  Since the unparser
                dies when emitting the statement
                (cmod->doSomething(EtherAppSrv__create))->setOwner();
                it is the statement that precedes it that is
                the last line of rose_bug.cc.
*/

class cModuleType
{
public:
  cModuleType(void (*cf)()) { }
  cModuleType *doSomething(void (*cf)()) { return 0; }
  void setOwner();
};

void EtherAppSrv__create()
{
}


static void __EtherAppSrv__mod_code()
{
  // The commented code also causes an assertion failure. 
  // Evidently, the assertion failure is not dependent
  // on an invocation of a constructor.
  //  (new cModuleType(EtherAppSrv__create))->setOwner();
  cModuleType *cmod;
  (cmod->doSomething(EtherAppSrv__create))->setOwner();
}

