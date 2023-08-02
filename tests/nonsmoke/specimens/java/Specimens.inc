# -*- makefile -*-

# All variables in this makefile begin with "nonsmoke_specimens_java_" (since that's where this file is located in the
# directory hierarchy.

nonsmoke_specimens_java_directory = $(top_srcdir)/tests/nonsmoke/specimens/java

#------------------------------------------------------------------------------------------------------------------------
# java class file specimens

nonsmoke_specimens_java_classes =		\
    Test1.class					\
    Worker.class				\
    Test2.class					\
    Worker1.class				\
    Worker2.class				\
    WorkerInterface.class			\
    Test3.class					\
    Generic.class				\
    Test4.class					\
    NestMembersAttribute.class			\
    NestMembersAttribute__InputReader.class
