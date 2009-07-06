#include "CppRuntimeSystem.h"
#include <cassert>

#include <sstream>
#include <iomanip>
using namespace std;




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

void RuntimeSystem::violationHandler(RuntimeViolation::Type v, const std::string & description)  throw (RuntimeViolation)
{
    RuntimeViolation vio(v,description);
    violationHandler(vio);
}

void RuntimeSystem::violationHandler(RuntimeViolation & vio)  throw (RuntimeViolation)
{
    vio.setPosition(curPos);

    (*defaultOutStr) << vio  << endl;

    throw vio;
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

    //FIXME get size of variable out of type information
    //      just allocate (wronly) always 4 bytes here
    //FIXME no allocation when "SgArrayType"
    RuntimeSystem::instance()->createMemory(var->getAddress(),4);


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

void RuntimeSystem::registerFileOpen(FILE * file,const string & openendFile, int mode)
{
    fileManager.openFile(file, openendFile,mode, curPos);
}

void RuntimeSystem::registerFileClose(FILE* file)
{
    fileManager.closeFile(file);
}


void RuntimeSystem::checkFileAccess(FILE * file, bool read)
{
    fileManager.checkFileAccess(file,read);
}


void RuntimeSystem::clearStatus()
{
    memManager.clearStatus();
    fileManager.clearStatus();

    while(scope.size() > 0)
        endScope();

    assert(stack.size() ==0);

    curPos = SourcePosition();
    beginScope("Globals");
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



