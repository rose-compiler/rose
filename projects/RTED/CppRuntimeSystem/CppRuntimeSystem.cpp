#include "CppRuntimeSystem.h"
#include <cassert>

#include <sstream>
#include <iomanip>
using namespace std;



// -----------------------    VariablesType  --------------------------------------

VariablesType::VariablesType(const std::string & _name,
                             const std::string & _mangledName,
                             const std::string & _typeStr,
                             addr_type _address) :
    name(_name),
    mangledName(_mangledName),
    type(_typeStr),
    address(_address)
{
    //FIXME get size of variable out of type information
    //      just allocate (wronly) always 4 bytes here
	//FIXME no allocation when "SgArrayType"
    RuntimeSystem::instance()->createMemory(address,4);

}

VariablesType::~VariablesType()
{
    RuntimeSystem::instance()->freeMemory(address);
}

MemoryType * VariablesType::getAllocation() const
{
    MemoryManager * mm = RuntimeSystem::instance()->getMemManager();

    MemoryType * mt =mm->getMemoryType(address);
    assert(mt);
    //FIXME
    //assert(mtt->getSize() ==??? )
    return mt;
}



void VariablesType::print(ostream & os) const
{
    os << "0x" << hex <<setw(6) << setfill('0') << address << "\t" << name << "(" << mangledName <<")" << " Type: " << type  ;
}


ostream& operator<< (ostream &os, const VariablesType & m)
{
    m.print(os);
}



// -----------------------    RuntimeSystem  --------------------------------------

RuntimeSystem * RuntimeSystem::single = NULL;

RuntimeSystem* RuntimeSystem::instance()
{
    if(!single)
        single = new RuntimeSystem();

    return single;
}



RuntimeSystem::RuntimeSystem()
    : defaultOutStr(&cout)
{
    beginScope("Globals");
}


void RuntimeSystem::setOutputFile(const std::string & filename)
{
    if(outFile.is_open())
    {
        cerr << "Warning: RuntimeSystem::setOutputFile called twice";
        outFile.close();
    }

    outFile.open(filename.c_str(),ios::out | ios::app);
    if(outFile.is_open());
        defaultOutStr = &outFile;

}


void RuntimeSystem::violationHandler(Violation v, const string & desc)
{
    ostream & outstr = *defaultOutStr;

    outstr << "Runtime System detected violation at " << curPos << endl ;
    switch(v)
    {
        case DOUBLE_ALLOCATION:
            outstr << "Double Allocation fault" << endl << desc;
            abort();
            break;
        case INVALID_FREE:
            outstr << "Invalid Free" << endl << desc << endl;
            abort();
        case MEMORY_LEAK:
            outstr << "Memory Leaks" << endl << desc<< endl;
            abort();
            break;
        case EMPTY_ALLOCATION:
            outstr << "Empty allocation" << endl << desc << endl;
            abort();
            break;
        case INVALID_READ:
            outstr << "Invalid Read" << endl <<desc << endl;
            abort();
            break;
        case INVALID_WRITE:
            outstr << "Invalid Write" << endl << desc<< endl;
            abort();
            break;
        case DOUBLE_FILE_OPEN:
            outstr << "FileHandle registered twice" << endl << desc << endl;
            abort();
            break;
        case INVALID_FILE_ACCESS:
            outstr << "Invalid File Access" << endl << desc << endl;
            abort();
            break;
        case UNCLOSED_FILES:
            outstr << "Open Files at end of program" << endl << desc<< endl;
            abort();
            break;
        default:
            // handle all possible violations!
            assert(false);
    }
}


void RuntimeSystem::createVariable(addr_type address,
                                   const std::string & name,
                                   const std::string & mangledName,
                                   const std::string & typeString)
{
    createVariable(new VariablesType(name,mangledName,typeString,address));
}

void RuntimeSystem::createVariable(VariablesType * var)
{
    // every variable has to part of scope
    assert(scope.size() >0);
    stack.push_back(var);
}

void RuntimeSystem::createMemory(addr_type startAddress, size_t size)
{
    // the created MemoryType is freed by memory manager
    memManager.allocateMemory(new MemoryType(startAddress,size,curPos));
}


void RuntimeSystem::freeMemory(addr_type startAddress)
{
    memManager.freeMemory(startAddress);
}


void RuntimeSystem::checkMemRead(addr_type addr, size_t size)
{
    memManager.checkRead(addr,size);
}

void RuntimeSystem::checkMemWrite(addr_type addr, size_t size)
{
    memManager.checkWrite(addr,size);
}


void RuntimeSystem::beginScope(const std::string & name)
{
    scope.push_back(ScopeInfo(name,stack.size()));
}

void RuntimeSystem::endScope()
{
    ScopeInfo lastScope = scope.back();
    scope.pop_back();

    for(int i=stack.size()-1; i >= lastScope.stackIndex ; i--)
    {
        delete stack.back();
        stack.pop_back();
    }
    assert(stack.size() == lastScope.stackIndex);
}



void RuntimeSystem::doProgramExitChecks()
{
    // Check for memory leaks
    memManager.checkForNonFreedMem();
    fileManager.checkForOpenFiles();
}




void RuntimeSystem::printStack(ostream & os) const
{
    os << endl;
    os << "------------------------------- Stack Status --------------------------------------" << endl << endl;

    for(int sc=0; sc < scope.size(); sc++)
    {
        os << scope[sc].name << ":" << endl;

        int endIndex = (sc == scope.size()-1) ? stack.size() : scope[sc+1].stackIndex;

        for(int i=scope[sc].stackIndex; i< endIndex; i++)
             os << "\t" << *(stack[i]) << endl;

    }

    os << endl;
}


// -----------------------    Testing   --------------------------------------

#if 1

    void testSuccessfulMallocFree()
    {
        RuntimeSystem * rs = RuntimeSystem::instance();
        rs->checkpoint(SourcePosition("TestSuccessfulMallocFreeFile",1,1));

        rs->createMemory(42,10);

        cout << "After creation" << endl;
        rs->printMemStatus(cout);

        rs->freeMemory(42);

        cout << "After free" << endl;
        rs->printMemStatus(cout);
    }

    void testFreeInsideBlock()
    {
        RuntimeSystem * rs = RuntimeSystem::instance();
        rs->checkpoint(SourcePosition("testFreeInsideBlock"));

        rs->createMemory(42,10);
        rs->freeMemory(44);
    }

    void testInvalidFree()
    {
        RuntimeSystem * rs = RuntimeSystem::instance();
        rs->checkpoint(SourcePosition("testInvalidFree"));

        rs->createMemory(42,10);
        rs->freeMemory(500);

    }

    void testDoubleAllocation()
    {
        RuntimeSystem * rs = RuntimeSystem::instance();
        rs->checkpoint(SourcePosition("testDoubleAllocation"));

        rs->createMemory(42,10);
        rs->createMemory(45,10);
    }

    void testStack()
    {
        addr_type addr=0;

        RuntimeSystem * rs = RuntimeSystem::instance();
        rs->createVariable(addr+=4,"GlobalVar1","MangledGlobal1","SgInt");
        rs->createVariable(addr+=4,"GlobalVar2","MangledGlobal2","SgDouble");

        cout << endl << endl << "After Globals" << endl;

        rs->printStack(cout);
        rs->printMemStatus(cout);

        rs->beginScope("Function1");
        rs->createVariable(addr+=4,"Function1Var1","Mangled","SgInt");
        rs->createVariable(addr+=4,"Fucntion1Var2","Mangled","SgDouble");

        rs->checkMemWrite(2348080,4);
        rs->checkMemRead(addr-4,4);

        cout << endl << endl << "After Function1" << endl;
        rs->printStack(cout);
        rs->printMemStatus(cout);


        rs->endScope();
        cout << endl << endl << "After return of function" << endl;
        rs->printStack(cout);
        rs->printMemStatus(cout);
    }



    int main(int argc, char ** argv)
    {
        testStack();

        cout << "Tests successful" << endl;
        return 0;
    }

#endif
