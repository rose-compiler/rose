#include "CppRuntimeSystem.h"
#include <cassert>

#include <sstream>
#include <iomanip>
using namespace std;


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



// --------------------- Mem Checking ---------------------------------


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


// --------------------- Scopes ---------------------------------


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


// --------------------- Stack Variables ---------------------------------


void RuntimeSystem::createVariable(addr_type address,
                                   const std::string & name,
                                   const std::string & mangledName,
                                   const std::string & typeString,
                                   size_t size)
{
    createVariable(new VariablesType(name,mangledName,typeString,address,size));
}

void RuntimeSystem::createVariable(VariablesType * var)
{
    // Track the memory area where the variable is stored
    // special case when static array, then createMemory is called anyway
    if(var->getType() != "SgArrayType")
        RuntimeSystem::instance()->createMemory(var->getAddress(),var->getSize());


    // every variable has to part of scope
    assert(scope.size() >0);
    stack.push_back(var);
}


// --------------------- Pointer Tracking---------------------------------

void RuntimeSystem::createPointer(const string & varName, addr_type targetAddress)
{
    VariablesType * var = findVarByName(varName);
    assert(var); // create the variable first!
    var->setPointerTarget(targetAddress,false);
}


void RuntimeSystem::registerPointerChange(const string & varName, addr_type targetAddress)
{
    VariablesType * var = findVarByName(varName);
    assert(var); // create the variable first!
    var->setPointerTarget(targetAddress,true);
}

VariablesType * RuntimeSystem::findVarByName(const string & name)
{
    for(int i=0; i< stack.size(); i++)
        if(name == stack[i]->getName() )
            return stack[i];


    return NULL;
}

// --------------------- File Management ---------------------------------

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



// --------------------- Violations ---------------------------------


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



void RuntimeSystem::doProgramExitChecks()
{
    // Check for memory leaks
    memManager.checkForNonFreedMem();
    fileManager.checkForOpenFiles();
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




// --------------------- Output Handling ---------------------------------


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



