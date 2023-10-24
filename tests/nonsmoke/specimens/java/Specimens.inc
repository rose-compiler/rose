# -*- makefile -*-

# All variables in this makefile begin with "nonsmoke_specimens_java_" (since that's where this file is located in the
# directory hierarchy.

nonsmoke_specimens_java_directory = $(top_srcdir)/tests/nonsmoke/specimens/java

#------------------------------------------------------------------------------------------------------------------------
# java class file specimens

nonsmoke_specimens_java_classes =		\
    T1_Test.class T1_Worker.class		\
    T2_Test.class T2_Worker1.class		\
      T2_Worker2.class T2_WorkerInterface.class	\
    T3_Test.class T3_Generic.class		\
    T4_Test.class				\
    T5_GiantFunction.class			\
    T6_MultiArray.class				\
    T7_TestNull.class				\
    T8_NestMembersAttribute.class		\
      T8_NestMembersAttribute__InputReader.class\
    T9_MembersOnly.class T9_Bottom.class	\
      T9_Middle.class T9_Base.class		\
    T10_TestInterface.class			\
      T10_CompleteChild.class T10_PartialParent.class 	\
      T10_InterfaceImplOther.class T10_InterfaceImpl.class T10_TheInterface.class	\
    T11_MutualCalls.class			\
      T11_WorkerA.class T11_WorkerB.class	\
    Worker_TT.class				\
    CPrint.class
