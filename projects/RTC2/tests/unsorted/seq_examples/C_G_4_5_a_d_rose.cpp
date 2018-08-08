namespace RTED
{
}
#include "RuntimeSystem_ParsingWorkaround.h"
/*
********************************************************************************
     Copyright (c) 2005 Iowa State University, Glenn Luecke, James Coyle, James Hoekstra, Marina Kraeva, Olga Taborskaia, Andre Wehe, Ying Xu, and Ziyu Zhang, All rights reserved.  Licensed under the Educational Community License version 1.0.  See the full agreement at http://rted.public.iastate.edu.
  
     Name of the test:  C_G_4_5_a_d.cpp
     Summary:           use "delete" with a pointer and a reference to the value of the pointer
     Test description:  the pointer is a default member variable; the allocation is in the constructor; one deallocation is in the destructor; the reference declaration and the additional deallocation by using the reference are in a member function; "double" is used for the base datatype of the pointer
     Error line:        55
     Support files:     Not needed
     Env. requirements: Not needed
     Keywords:          constructor, member function, deallocate twice, default member variable, delete, destructor, pointer, reference, value
     Last modified:     07/08/2005 15:27
     Programmer:        Andre Wehe, Iowa State University
********************************************************************************
*/
#include <iostream>
#include <cstdlib>
#ifndef EXITCODE_OK
#define EXITCODE_OK 1
#endif

class TestClass 
{
// this is the declaration of the pointer
public: double *ptrA;
TestClass();
~TestClass();
void runA();
}
;

TestClass::TestClass()
{
// 
// RS: Create Variable, parameters: type, basetype, indirection_level, address, size, filename, line, linetransformed
rted_CreateObject(((struct rted_TypeDesc ){"TestClass", "", {0, 0}}),rted_Addr(((char *)(this))),sizeof(( *(this))),((struct rted_SourceInfo ){"/local/home/rvanka/research/compilers/rose/rose_build/projects/RTED/rted_source/C++/allocation_deallocation_errors/C_G_4_5_a_d.cpp", 43, __LINE__}));
// memory allocation
rted_EnterScope("::std::basic_ostream < char , char_traits< char > > ::__ostream_type&(::std::basic_ostream < char , char_traits< char > > ::__ostream_type&(*)(__ostream_type &))");
rted_EnterScope("class ::std::basic_ostream < char , char_traits< char > > &(class basic_ostream< char , char_traits< char > > &, const char *)");
( *(&std::cout)<<"memory allocation for ptrA") << std::endl< char  , std::char_traits< char  >  > ;
rted_ExitScope(1,((struct rted_SourceInfo ){"/home/rvanka/research/compilers/rose/rose_build/include-staging/g++_HEADERS/hdrs1/bits/ostream.tcc", 761, __LINE__}));
rted_ExitScope(1,((struct rted_SourceInfo ){"/home/rvanka/research/compilers/rose/rose_build/include-staging/g++_HEADERS/hdrs1/bits/ostream.tcc", 72, __LINE__}));
// 
// RS : roseCheckIfThisNULL, parameters : (ThisExp, filename, line, line transformed, error Str)
rted_CheckIfThisNULL((this),((struct rted_SourceInfo ){"/local/home/rvanka/research/compilers/rose/rose_build/projects/RTED/rted_source/C++/allocation_deallocation_errors/C_G_4_5_a_d.cpp", 46, __LINE__}));
(this) -> ptrA = (new double );
// 
// RS : Create Array Variable, parameters : (name, manglname, typr, basetype, address, sizeof(type), array size, fromMalloc, filename, linenr, linenrTransformed, dimension info ...)
rted_AllocMem(((struct rted_TypeDesc ){"SgPointerType", "SgTypeDouble", {1, 0}}),rted_Addr(((char *)(&(this) -> ptrA))),sizeof((this) -> ptrA),akCxxNew,0L,((unsigned long )(sizeof(double ))),"",((struct rted_SourceInfo ){"/local/home/rvanka/research/compilers/rose/rose_build/projects/RTED/rted_source/C++/allocation_deallocation_errors/C_G_4_5_a_d.cpp", 46, __LINE__}));
// 
// RS : Init Variable, parameters : (tpye, basetype, class_name, address, size, ismalloc, is_pointer_change, filename, line, linenrTransformed, error line)
rted_InitVariable(((struct rted_TypeDesc ){"SgPointerType", "SgTypeDouble", {1, 0}}),rted_Addr(((char *)(&(this) -> ptrA))),sizeof((this) -> ptrA),1,"",((struct rted_SourceInfo ){"/local/home/rvanka/research/compilers/rose/rose_build/projects/RTED/rted_source/C++/allocation_deallocation_errors/C_G_4_5_a_d.cpp", 46, __LINE__}));
// initialize
rted_EnterScope("int()");
// 
// RS : roseCheckIfThisNULL, parameters : (ThisExp, filename, line, line transformed, error Str)
rted_CheckIfThisNULL((this),((struct rted_SourceInfo ){"/local/home/rvanka/research/compilers/rose/rose_build/projects/RTED/rted_source/C++/allocation_deallocation_errors/C_G_4_5_a_d.cpp", 49, __LINE__}));
// 
// RS : Access Variable, parameters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str)
rted_AccessVariable(rted_Addr(((char *)(&((this) -> ptrA)))),sizeof(((this) -> ptrA)),rted_Addr(((char *)((this) -> ptrA))),sizeof(( *((this) -> ptrA))),3,((struct rted_SourceInfo ){"/local/home/rvanka/research/compilers/rose/rose_build/projects/RTED/rted_source/C++/allocation_deallocation_errors/C_G_4_5_a_d.cpp", 49, __LINE__}));
// 
// RS : roseCheckIfThisNULL, parameters : (ThisExp, filename, line, line transformed, error Str)
rted_CheckIfThisNULL((this),((struct rted_SourceInfo ){"/local/home/rvanka/research/compilers/rose/rose_build/projects/RTED/rted_source/C++/allocation_deallocation_errors/C_G_4_5_a_d.cpp", 49, __LINE__}));
rted_AssertFunctionSignature("rand",1,((struct rted_TypeDesc []){{"SgTypeInt", "", {0, 0}}}),((struct rted_SourceInfo ){"/local/home/rvanka/research/compilers/rose/rose_build/projects/RTED/rted_source/C++/allocation_deallocation_errors/C_G_4_5_a_d.cpp", 49, __LINE__}));
 *((this) -> ptrA) = (rand());
// 
// RS : Init Variable, parameters : (tpye, basetype, class_name, address, size, ismalloc, is_pointer_change, filename, line, linenrTransformed, error line)
rted_InitVariable(((struct rted_TypeDesc ){"SgTypeDouble", "", {0, 0}}),rted_Addr(((char *)((this) -> ptrA))),sizeof(( *((this) -> ptrA))),0,"",((struct rted_SourceInfo ){"/local/home/rvanka/research/compilers/rose/rose_build/projects/RTED/rted_source/C++/allocation_deallocation_errors/C_G_4_5_a_d.cpp", 49, __LINE__}));
rted_ExitScope(1,((struct rted_SourceInfo ){"/local/home/rvanka/research/compilers/rose/rose_build/projects/RTED/rted_source/C++/allocation_deallocation_errors/C_G_4_5_a_d.cpp", 49, __LINE__}));
}

TestClass::~TestClass()
{
// memory deallocation
rted_EnterScope("::std::basic_ostream < char , char_traits< char > > ::__ostream_type&(::std::basic_ostream < char , char_traits< char > > ::__ostream_type&(*)(__ostream_type &))");
rted_EnterScope("class ::std::basic_ostream < char , char_traits< char > > &(class basic_ostream< char , char_traits< char > > &, const char *)");
( *(&std::cout)<<"memory deallocation of ptrA") << std::endl< char  , std::char_traits< char  >  > ;
rted_ExitScope(1,((struct rted_SourceInfo ){"/home/rvanka/research/compilers/rose/rose_build/include-staging/g++_HEADERS/hdrs1/bits/ostream.tcc", 761, __LINE__}));
rted_ExitScope(1,((struct rted_SourceInfo ){"/home/rvanka/research/compilers/rose/rose_build/include-staging/g++_HEADERS/hdrs1/bits/ostream.tcc", 72, __LINE__}));
// 
// RS : roseCheckIfThisNULL, parameters : (ThisExp, filename, line, line transformed, error Str)
rted_CheckIfThisNULL((this),((struct rted_SourceInfo ){"/local/home/rvanka/research/compilers/rose/rose_build/projects/RTED/rted_source/C++/allocation_deallocation_errors/C_G_4_5_a_d.cpp", 55, __LINE__}));
// 
// RS : Access Variable, parameters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str)
rted_AccessVariable(rted_Addr(((char *)(&((this) -> ptrA)))),sizeof(((this) -> ptrA)),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/local/home/rvanka/research/compilers/rose/rose_build/projects/RTED/rted_source/C++/allocation_deallocation_errors/C_G_4_5_a_d.cpp", 55, __LINE__}));
rted_FreeMemory(rted_Addr(((this) -> ptrA)),akCxxNew,((struct rted_SourceInfo ){"/local/home/rvanka/research/compilers/rose/rose_build/projects/RTED/rted_source/C++/allocation_deallocation_errors/C_G_4_5_a_d.cpp", 55, __LINE__}));
delete ((this) -> ptrA);
}

void TestClass::runA()
{
// reference to the value of the pointer
// 
// RS : roseCheckIfThisNULL, parameters : (ThisExp, filename, line, line transformed, error Str)
rted_CheckIfThisNULL((this),((struct rted_SourceInfo ){"/local/home/rvanka/research/compilers/rose/rose_build/projects/RTED/rted_source/C++/allocation_deallocation_errors/C_G_4_5_a_d.cpp", 60, __LINE__}));
// 
// RS : Access Variable, parameters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str)
rted_AccessVariable(rted_Addr(((char *)((this) -> ptrA))),sizeof(( *((this) -> ptrA))),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/local/home/rvanka/research/compilers/rose/rose_build/projects/RTED/rted_source/C++/allocation_deallocation_errors/C_G_4_5_a_d.cpp", 60, __LINE__}));
// 
// RS : roseCheckIfThisNULL, parameters : (ThisExp, filename, line, line transformed, error Str)
rted_CheckIfThisNULL((this),((struct rted_SourceInfo ){"/local/home/rvanka/research/compilers/rose/rose_build/projects/RTED/rted_source/C++/allocation_deallocation_errors/C_G_4_5_a_d.cpp", 60, __LINE__}));
// 
// RS : Access Variable, parameters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str)
rted_AccessVariable(rted_Addr(((char *)(&((this) -> ptrA)))),sizeof(((this) -> ptrA)),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/local/home/rvanka/research/compilers/rose/rose_build/projects/RTED/rted_source/C++/allocation_deallocation_errors/C_G_4_5_a_d.cpp", 60, __LINE__}));
double &refA =  *((this) -> ptrA);
// outsmart dead code elimination
rted_EnterScope("::std::basic_ostream < char , char_traits< char > > ::__ostream_type&(::std::basic_ostream < char , char_traits< char > > ::__ostream_type&(*)(__ostream_type &))");
rted_EnterScope("::std::basic_ostream < char , char_traits< char > > ::__ostream_type&(double )");
rted_EnterScope("class ::std::basic_ostream < char , char_traits< char > > &(class basic_ostream< char , char_traits< char > > &, const char *)");
rted_EnterScope("::std::basic_ostream < char , char_traits< char > > ::__ostream_type&(const void *)");
rted_EnterScope("class ::std::basic_ostream < char , char_traits< char > > &(class basic_ostream< char , char_traits< char > > &, const char *)");
rted_EnterScope("::std::basic_ostream < char , char_traits< char > > ::__ostream_type&(const void *)");
rted_EnterScope("class ::std::basic_ostream < char , char_traits< char > > &(class basic_ostream< char , char_traits< char > > &, const char *)");
// 
// RS : roseCheckIfThisNULL, parameters : (ThisExp, filename, line, line transformed, error Str)
rted_CheckIfThisNULL((this),((struct rted_SourceInfo ){"/local/home/rvanka/research/compilers/rose/rose_build/projects/RTED/rted_source/C++/allocation_deallocation_errors/C_G_4_5_a_d.cpp", 63, __LINE__}));
// 
// RS : Access Variable, parameters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str)
rted_AccessVariable(rted_Addr(((char *)((this) -> ptrA))),sizeof(( *((this) -> ptrA))),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/local/home/rvanka/research/compilers/rose/rose_build/projects/RTED/rted_source/C++/allocation_deallocation_errors/C_G_4_5_a_d.cpp", 63, __LINE__}));
// 
// RS : roseCheckIfThisNULL, parameters : (ThisExp, filename, line, line transformed, error Str)
rted_CheckIfThisNULL((this),((struct rted_SourceInfo ){"/local/home/rvanka/research/compilers/rose/rose_build/projects/RTED/rted_source/C++/allocation_deallocation_errors/C_G_4_5_a_d.cpp", 63, __LINE__}));
// 
// RS : Access Variable, parameters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str)
rted_AccessVariable(rted_Addr(((char *)(&(this) -> ptrA))),sizeof((this) -> ptrA),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/local/home/rvanka/research/compilers/rose/rose_build/projects/RTED/rted_source/C++/allocation_deallocation_errors/C_G_4_5_a_d.cpp", 63, __LINE__}));
// 
// RS : roseCheckIfThisNULL, parameters : (ThisExp, filename, line, line transformed, error Str)
rted_CheckIfThisNULL((this),((struct rted_SourceInfo ){"/local/home/rvanka/research/compilers/rose/rose_build/projects/RTED/rted_source/C++/allocation_deallocation_errors/C_G_4_5_a_d.cpp", 63, __LINE__}));
// 
// RS : Access Variable, parameters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str)
rted_AccessVariable(rted_Addr(((char *)(&((this) -> ptrA)))),sizeof(((this) -> ptrA)),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/local/home/rvanka/research/compilers/rose/rose_build/projects/RTED/rted_source/C++/allocation_deallocation_errors/C_G_4_5_a_d.cpp", 63, __LINE__}));
// 
// RS : roseCheckIfThisNULL, parameters : (ThisExp, filename, line, line transformed, error Str)
rted_CheckIfThisNULL((this),((struct rted_SourceInfo ){"/local/home/rvanka/research/compilers/rose/rose_build/projects/RTED/rted_source/C++/allocation_deallocation_errors/C_G_4_5_a_d.cpp", 63, __LINE__}));
// 
// RS : Access Variable, parameters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str)
rted_AccessVariable(rted_Addr(((char *)(&((this) -> ptrA)))),sizeof(((this) -> ptrA)),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/local/home/rvanka/research/compilers/rose/rose_build/projects/RTED/rted_source/C++/allocation_deallocation_errors/C_G_4_5_a_d.cpp", 63, __LINE__}));
(((( *(&std::cout)<<"ptrA ") << (&(this) -> ptrA)<<", ") << ((this) -> ptrA)<<", ") <<  *((this) -> ptrA)) << std::endl< char  , std::char_traits< char  >  > ;
rted_ExitScope(1,((struct rted_SourceInfo ){"/home/rvanka/research/compilers/rose/rose_build/include-staging/g++_HEADERS/hdrs1/bits/ostream.tcc", 761, __LINE__}));
rted_ExitScope(1,((struct rted_SourceInfo ){"/home/rvanka/research/compilers/rose/rose_build/include-staging/g++_HEADERS/hdrs1/bits/ostream.tcc", 424, __LINE__}));
rted_ExitScope(1,((struct rted_SourceInfo ){"/home/rvanka/research/compilers/rose/rose_build/include-staging/g++_HEADERS/hdrs1/bits/ostream.tcc", 761, __LINE__}));
rted_ExitScope(1,((struct rted_SourceInfo ){"/home/rvanka/research/compilers/rose/rose_build/include-staging/g++_HEADERS/hdrs1/bits/ostream.tcc", 424, __LINE__}));
rted_ExitScope(1,((struct rted_SourceInfo ){"/home/rvanka/research/compilers/rose/rose_build/include-staging/g++_HEADERS/hdrs1/bits/ostream.tcc", 761, __LINE__}));
rted_ExitScope(1,((struct rted_SourceInfo ){"/home/rvanka/research/compilers/rose/rose_build/include-staging/g++_HEADERS/hdrs1/bits/ostream.tcc", 378, __LINE__}));
rted_ExitScope(1,((struct rted_SourceInfo ){"/home/rvanka/research/compilers/rose/rose_build/include-staging/g++_HEADERS/hdrs1/bits/ostream.tcc", 72, __LINE__}));
// outsmart dead code elimination
rted_EnterScope("::std::basic_ostream < char , char_traits< char > > ::__ostream_type&(::std::basic_ostream < char , char_traits< char > > ::__ostream_type&(*)(__ostream_type &))");
rted_EnterScope("::std::basic_ostream < char , char_traits< char > > ::__ostream_type&(double )");
rted_EnterScope("class ::std::basic_ostream < char , char_traits< char > > &(class basic_ostream< char , char_traits< char > > &, const char *)");
rted_EnterScope("::std::basic_ostream < char , char_traits< char > > ::__ostream_type&(const void *)");
rted_EnterScope("class ::std::basic_ostream < char , char_traits< char > > &(class basic_ostream< char , char_traits< char > > &, const char *)");
// 
// RS : Access Variable, parameters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str)
rted_AccessVariable(rted_Addr(((char *)(&refA))),sizeof(refA),rted_Addr(0),sizeof(0L),1,((struct rted_SourceInfo ){"/local/home/rvanka/research/compilers/rose/rose_build/projects/RTED/rted_source/C++/allocation_deallocation_errors/C_G_4_5_a_d.cpp", 66, __LINE__}));
((( *(&std::cout)<<"refA ") << (&refA)<<", ") << refA) << std::endl< char  , std::char_traits< char  >  > ;
rted_ExitScope(1,((struct rted_SourceInfo ){"/home/rvanka/research/compilers/rose/rose_build/include-staging/g++_HEADERS/hdrs1/bits/ostream.tcc", 761, __LINE__}));
rted_ExitScope(1,((struct rted_SourceInfo ){"/home/rvanka/research/compilers/rose/rose_build/include-staging/g++_HEADERS/hdrs1/bits/ostream.tcc", 424, __LINE__}));
rted_ExitScope(1,((struct rted_SourceInfo ){"/home/rvanka/research/compilers/rose/rose_build/include-staging/g++_HEADERS/hdrs1/bits/ostream.tcc", 761, __LINE__}));
rted_ExitScope(1,((struct rted_SourceInfo ){"/home/rvanka/research/compilers/rose/rose_build/include-staging/g++_HEADERS/hdrs1/bits/ostream.tcc", 378, __LINE__}));
rted_ExitScope(1,((struct rted_SourceInfo ){"/home/rvanka/research/compilers/rose/rose_build/include-staging/g++_HEADERS/hdrs1/bits/ostream.tcc", 72, __LINE__}));
// memory deallocation
rted_EnterScope("::std::basic_ostream < char , char_traits< char > > ::__ostream_type&(::std::basic_ostream < char , char_traits< char > > ::__ostream_type&(*)(__ostream_type &))");
rted_EnterScope("class ::std::basic_ostream < char , char_traits< char > > &(class basic_ostream< char , char_traits< char > > &, const char *)");
( *(&std::cout)<<"memory deallocation by using refA") << std::endl< char  , std::char_traits< char  >  > ;
rted_ExitScope(1,((struct rted_SourceInfo ){"/home/rvanka/research/compilers/rose/rose_build/include-staging/g++_HEADERS/hdrs1/bits/ostream.tcc", 761, __LINE__}));
rted_ExitScope(1,((struct rted_SourceInfo ){"/home/rvanka/research/compilers/rose/rose_build/include-staging/g++_HEADERS/hdrs1/bits/ostream.tcc", 72, __LINE__}));
rted_FreeMemory(rted_Addr(&refA),akCxxNew,((struct rted_SourceInfo ){"/local/home/rvanka/research/compilers/rose/rose_build/projects/RTED/rted_source/C++/allocation_deallocation_errors/C_G_4_5_a_d.cpp", 70, __LINE__}));
delete &refA;
}

extern int RuntimeSystem_original_main(int argc,char **argv,char **envp)
{
rted_ConfirmFunctionSignature("RuntimeSystem_original_main",1,((struct rted_TypeDesc []){{"SgTypeInt", "", {0, 0}}}));
// 
// RS : Create Variable, parameters : (name, mangl_name, type, basetype, address, sizeof, initialized, fileOpen, classname, filename, linenr, linenrTransformed)
rted_CreateVariable(((struct rted_TypeDesc ){"SgPointerType", "SgTypeChar", {2, 0}}),rted_Addr(((char *)(&envp))),sizeof(envp),1,akStack,"envp","envp","",((struct rted_SourceInfo ){"transformation", -1, __LINE__}));
// 
// RS : Create Variable, parameters : (name, mangl_name, type, basetype, address, sizeof, initialized, fileOpen, classname, filename, linenr, linenrTransformed)
rted_CreateVariable(((struct rted_TypeDesc ){"SgPointerType", "SgTypeChar", {2, 0}}),rted_Addr(((char *)(&argv))),sizeof(argv),1,akStack,"argv","argv","",((struct rted_SourceInfo ){"transformation", -1, __LINE__}));
// 
// RS : Create Variable, parameters : (name, mangl_name, type, basetype, address, sizeof, initialized, fileOpen, classname, filename, linenr, linenrTransformed)
rted_CreateVariable(((struct rted_TypeDesc ){"SgTypeInt", "", {0, 0}}),rted_Addr(((char *)(&argc))),sizeof(argc),1,akStack,"argc","argc","",((struct rted_SourceInfo ){"transformation", -1, __LINE__}));
rted_RegisterTypeCall("TestClass","SgClassType",0,sizeof(class TestClass ),((struct rted_SourceInfo ){"/local/home/rvanka/research/compilers/rose/rose_build/projects/RTED/rted_source/C++/allocation_deallocation_errors/C_G_4_5_a_d.cpp", 74, __LINE__}),4,"TestClass__scope__ptrA",((struct rted_TypeDesc ){"SgPointerType", "SgTypeDouble", {1, 0}}),((size_t )(&( *((class TestClass *)0)).TestClass::ptrA)),sizeof(( *((class TestClass *)0)).TestClass::ptrA));
class TestClass testclassA;
// 
// RS : Create Variable, parameters : (name, mangl_name, type, basetype, address, sizeof, initialized, fileOpen, classname, filename, linenr, linenrTransformed)
rted_CreateVariable(((struct rted_TypeDesc ){"TestClass", "", {0, 0}}),rted_Addr(((char *)(&testclassA))),sizeof(testclassA),0,akStack,"testclassA","testclassA","TestClass",((struct rted_SourceInfo ){"transformation", -1, __LINE__}));
rted_EnterScope("void()");
testclassA. runA ();
rted_ExitScope(1,((struct rted_SourceInfo ){"/local/home/rvanka/research/compilers/rose/rose_build/projects/RTED/rted_source/C++/allocation_deallocation_errors/C_G_4_5_a_d.cpp", 71, __LINE__}));
int rstmt = 0;
// RS : Insert Finalizing Call to Runtime System to check if error was detected (needed for automation)
rted_Checkpoint(((struct rted_SourceInfo ){"/local/home/rvanka/research/compilers/rose/rose_build/projects/RTED/rted_source/C++/allocation_deallocation_errors/C_G_4_5_a_d.cpp", 77, __LINE__}));
return rstmt;
}

int main(int argc,char **argv,char **envp)
{
rted_UpcAllInitialize();
int exit_code = RuntimeSystem_original_main(argc,argv,envp);
rted_Close("RuntimeSystem.cpp:main");
return exit_code;
}
