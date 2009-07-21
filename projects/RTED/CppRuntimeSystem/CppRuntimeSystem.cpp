#include "CppRuntimeSystem.h"
#include <cassert>
#include <sstream>
#include <iomanip>

#ifdef ROSE_WITH_ROSEQT
#include "DebuggerQt/RtedDebug.h"
#endif


using namespace std;




RuntimeSystem * RuntimeSystem::single = NULL;

RuntimeSystem* RuntimeSystem::instance()
{
    if(!single)
        single = new RuntimeSystem();

    return single;
}

RuntimeSystem::RuntimeSystem()
    : defaultOutStr(&cout),testingMode(false),qtDebugger(false)
{
    beginScope("Globals");
}

void RuntimeSystem::checkpoint(const SourcePosition & pos)
{
    curPos = pos;

#ifdef ROSE_WITH_ROSEQT
    if(qtDebugger)
        RtedDebug::instance()->startGui();
#endif
}


// --------------------- Mem Checking ---------------------------------


void RuntimeSystem::createMemory(addr_type startAddress, size_t size, bool onStack)
{
    // the created MemoryType is freed by memory manager
    memManager.allocateMemory(new MemoryType(startAddress,size,curPos,onStack));
}


void RuntimeSystem::freeMemory(addr_type startAddress, bool onStack)
{
    memManager.freeMemory(startAddress, onStack);
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
    assert( scope.size() > 0);

    ScopeInfo lastScope = scope.back();
    scope.pop_back();

    vector<VariablesType * > toDelete;

    for(int i=stack.size()-1; i >= lastScope.stackIndex ; i--)
    {
        // First delete the variables then the pointer
        // otherwise there are lots of errors about no pointer to allocated mem-region
        if(stack.back()->isPointer())
            toDelete.push_back(stack.back());
        else
            delete stack.back();

        stack.pop_back();
    }
    for(int i=0; i<toDelete.size(); i++)
        delete toDelete[i];

    assert(stack.size() == lastScope.stackIndex);
}

int   RuntimeSystem::getScopeCount()  const
{
    return scope.size();
}

const std::string & RuntimeSystem::getScopeName(int i) const
{
    assert(i >=0 );
    assert(i < scope.size());
    return scope[i].name;
}

RuntimeSystem::VariableIter RuntimeSystem::variablesBegin(int i) const
{
    assert(i >=0 );
    assert(i < scope.size());

    return stack.begin() + scope[i].stackIndex;
}

RuntimeSystem::VariableIter RuntimeSystem::variablesEnd(int i) const
{
    assert(i >=0 );
    assert(i < scope.size());

    if(i-1 == scope.size())
        return stack.end();
    else
        return stack.begin() + scope[i+1].stackIndex;
}





// --------------------- Stack Variables ---------------------------------


void RuntimeSystem::createVariable(addr_type address,
                                   const std::string & name,
                                   const std::string & mangledName,
                                   const std::string & typeString,
                                   const std::string & pointerType)
{
    RsType * pt =NULL;


    if(pointerType.size() > 0)
    {
        pt = typeSystem.getTypeInfo(pointerType);
        if(!pt)
            cerr << "Couldn't find type " << pointerType << " when registering pointer-type" << endl;
    }


    createVariable(new VariablesType(name,mangledName,typeString,address,pt));
}

void RuntimeSystem::createVariable(VariablesType * var)
{
    // Track the memory area where the variable is stored
    // special case when static array, then createMemory is called anyway
    if(var->getType()->getName() != "SgArrayType")
    {
        createMemory(var->getAddress(),var->getSize(), true);
        // tell the memManager the type of this memory chunk
        var->getAllocation()->accessMemWithType(0,var->getType());
    }


    // every variable has to part of scope
    assert(scope.size() >0);
    stack.push_back(var);
}


// --------------------- Pointer Tracking---------------------------------


void RuntimeSystem::registerPointerChange(const string & varName, addr_type targetAddress, bool checks)
{
    VariablesType * var = findVarByName(varName);
    assert(var); // create the variable first!
    var->setPointerTarget(targetAddress,checks);
}


void RuntimeSystem::checkForSameChunk(addr_type addr1, addr_type addr2, const string & type)
{
    size_t typeSize = typeSystem.getTypeInfo(type)->getByteSize();
    MemoryType * mem1 = memManager.findContainingMem(addr1,typeSize);
    MemoryType * mem2 = memManager.findContainingMem(addr2,typeSize);


    if(mem1 != mem2 || !mem1 || !mem2)
    {
        stringstream ss;
        ss << "Pointer changed memory block from 0x" << hex << addr1 << " to "
                                                     << hex << addr2 << endl;

        if(!mem1)  ss << "No allocation found at addr1" << endl;
        if(!mem2)  ss << "No allocation found at addr2" << endl;

        ss << "Where the two memory regions are not the same" << endl;
        violationHandler(RuntimeViolation::INVALID_PTR_ASSIGN,ss.str());
        return;
    }

    int off1 = addr1 - mem1->getAddress();
    int off2 = addr2 - mem2->getAddress();
    string chunk1 = mem1->getTypeAt(off1,typeSize);
    string chunk2 = mem2->getTypeAt(off2,typeSize);

    if(chunk1 == chunk2 && mem1 ==mem2)
        return; //pointer just changed offset in an array


    stringstream ss;
    ss << "A pointer changed the memory area (array or variable) which it points to (may be an error)" << endl << endl;

    ss << "Region1:  " <<  chunk1 << " at offset " << off1 << " in this Mem-Region:" <<  endl
                          << *mem1 << endl;

    ss << "Region2: " << chunk2 << " at offset " << off2 << " in this Mem-Region:" <<  endl
                          << *mem2 << endl;

    violationHandler(RuntimeViolation::POINTER_CHANGED_MEMAREA,ss.str());
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

    if(testingMode)
    	throw vio;
    else
    	exit(0);
}



void RuntimeSystem::doProgramExitChecks()
{
    // exit global scope
    endScope();
    // allows you to call doProgramExitChecks but then keep going without first
    // calling clearStatus.  Convenient for testing, but not generally
    // recommended.
    beginScope("Globals");


    // Check for memory leaks
    memManager.checkForNonFreedMem();
    fileManager.checkForOpenFiles();
}


void RuntimeSystem::clearStatus()
{
    memManager.clearStatus();
    fileManager.clearStatus();
    typeSystem.clearStatus();

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



