#include "CppRuntimeSystem.h"
#include <cassert>

#include <sstream>
#include <iomanip>
using namespace std;

// ----------------------- SourcePosition --------------------------------------


SourcePosition::SourcePosition()
    : file("Unknown"),
      line1(-1),
      line2(-1)
{

}

SourcePosition::SourcePosition(const std::string & _file)
    : file(_file),
      line1(-1),
      line2(-1)
{}

SourcePosition::SourcePosition(const std::string & _file,int _line1, int _line2)
    : file(_file),
      line1(_line1),
      line2(_line2)
{}




ostream& operator<< (ostream &os, const SourcePosition & m)
{
    os << m.file << "(" << m.line1 << "," << m.line2 << ")";
    return os;
}


// -----------------------    MemoryType  --------------------------------------


MemoryType::MemoryType(addr_type _addr, size_t _size, const SourcePosition & _pos)
    : startAddress(_addr), size(_size), allocPos(_pos)
{
    // not memory has been initialized by default
    initialized.assign(size,false);
}

MemoryType::MemoryType(addr_type _addr, size_t _size,
                       const std::string & file, int line1, int line2)
    : startAddress(_addr),
      size(_size),
      allocPos(file,line1,line2)
{
}

MemoryType::MemoryType(addr_type addr)
    : startAddress(addr), size(0), allocPos("unknown",-1,-1)
{
}


bool MemoryType::containsAddress(addr_type queryAddress)
{
    return ( queryAddress >= startAddress &&
             queryAddress < startAddress+size );
}

bool MemoryType::containsMemArea(addr_type queryAddr, size_t querySize)
{
    return ( containsAddress(queryAddr) &&
             containsAddress(queryAddr+querySize-1) );
}

bool MemoryType::overlapsMemArea(addr_type queryAddr, size_t querySize)
{
    return ( containsAddress(queryAddr) ||
             containsAddress(queryAddr+querySize-1) );
}

bool MemoryType::operator< (const MemoryType & other) const
{
    return (startAddress < other.startAddress);
}


bool MemoryType::isInitialized(int offsetFrom, int offsetTo) const
{
    assert(offsetFrom<offsetTo);
    assert(offsetFrom < size && offsetTo <= size);

    for(int i= offsetFrom; i<offsetTo; i++)
        if(! initialized[i])
            return false;

    return true;
}

void MemoryType::initialize(int offsetFrom, int offsetTo)
{
    assert(offsetFrom<offsetTo);
    assert(offsetFrom < size && offsetTo <= size);

    for(int i=offsetFrom; i<offsetTo; i++)
        initialized[i]=true;
}


// extra print function because operator<< cannot be member-> no access to privates
void MemoryType::print(ostream & os) const
{
    std::string init = "Part. initialized";
    bool allInit  = true;
    bool noneInit = true;
    for(vector<bool>::const_iterator it = initialized.begin();
            it != initialized.end();
            ++it)
    {
        if( *it)
            noneInit=false;
        if (! *it)
            allInit=false;
    }

    if(allInit)
        init = "Initialized      ";
    if(noneInit)
        init = "Not initialized  ";



    os << "0x" << setfill('0')<< setw(6) << hex << startAddress
       << " Size " << dec << size <<  "\t" << init << "\tAllocated at " << allocPos ;

}

ostream& operator<< (ostream &os, const MemoryType & m)
{
    m.print(os);
    return os;
}




// -----------------------    MemoryManager  --------------------------------------

MemoryManager::MemoryManager()
{
}

MemoryManager::~MemoryManager()
{
    checkForNonFreedMem();

    // free all the allocation-types
    for(set<MemoryType*>::iterator i = mem.begin(); i != mem.end(); ++i)
        delete *i;
}


MemoryType * MemoryManager::findPossibleMemMatch(addr_type addr)
{
   if(mem.size()==0)
        return NULL;

    // search the memory chunk, we get the chunk which has equal or greater startAdress
    MemoryType compareObject (addr);
    MemoryTypeSet::iterator it = mem.lower_bound(&compareObject);

    // case 0: we are at the end and map.size>0, so return last element
    if(it == mem.end())
    {
        --it;
        return (*it);
    }

    //case 1: addr is startAdress of a chunk, exact match
    if((*it)->getAddress() == addr )
    {
        return (*it);
    }

    //case 2: addr is greater than a chunk startadress
    //        try to get next smaller one
    assert( (*it)->getAddress() > addr);

    if( it == mem.begin())
        return NULL;

    --it;

    return (*it);
}

MemoryType * MemoryManager::findContainingMem(addr_type addr, size_t size)
{
    MemoryType * res = findPossibleMemMatch(addr);
    if(!res)
        return NULL;

    return res->containsMemArea(addr,size) ?  res  : NULL;
}

MemoryType * MemoryManager::findOverlappingMem(addr_type addr, size_t size)
{
    MemoryType * res = findPossibleMemMatch(addr);
    if(!res)
        return NULL;

    return res->overlapsMemArea(addr,size) ?  res  : NULL;
}



void MemoryManager::allocateMemory(MemoryType * alloc)
{
    RuntimeSystem * rs = RuntimeSystem::instance();
    if(alloc->getSize()==0 )
    {
        rs->violationHandler(RuntimeSystem::EMPTY_ALLOCATION,"Tried to call malloc/new with size 0\n");
        return;
    }


    MemoryType * m = findOverlappingMem(alloc->getAddress(), alloc->getSize());
    if(m)
    {
        // the start address of new chunk lies in already allocated area
        rs->violationHandler(RuntimeSystem::DOUBLE_ALLOCATION);
        return;
    }

    mem.insert(alloc);
}


void MemoryManager::freeMemory(addr_type addr)
{
    RuntimeSystem * rs = RuntimeSystem::instance();

    MemoryType * m = findContainingMem(addr,1);

    // free on unknown address
    if(m == NULL)
    {
        stringstream desc;
        desc << "Free was called with adress " << addr << endl;
        desc << "Allocated Memory Regions:" << endl;
        print(desc);
        rs->violationHandler(RuntimeSystem::INVALID_FREE,desc.str());
        return;
    }

    // free inside an allocated memory block
    if(m->getAddress() != addr)
    {
        stringstream desc;
        desc << "Free was called with an address inside of allocated block (Offset:"
             << addr - m->getAddress() <<")" << endl;
        desc << "Allocated Block: " << *m << endl;

        rs->violationHandler(RuntimeSystem::INVALID_FREE, desc.str());
        return;
    }

    // successful free, erase allocation info from map
    mem.erase(m);
}



void MemoryManager::checkRead(addr_type addr, size_t size)
{
    RuntimeSystem * rs = RuntimeSystem::instance();


    MemoryType * mt = findContainingMem(addr,size);
    if(!mt)
    {
        stringstream desc;
        desc << "Trying to read from non-allocated MemoryRegion (Address " << addr <<")" << endl;

        MemoryType * possMatch = findPossibleMemMatch(addr);
        if(possMatch)
            desc << "Did you try to read this region:" << endl << *possMatch << endl;

        rs->violationHandler(RuntimeSystem::INVALID_READ,desc.str());
        return;
    }

    assert(addr >= mt->getAddress());
    int from = addr - mt->getAddress();

    if(! mt->isInitialized(from, from +size) )
    {
        stringstream desc;
        desc << "Trying to read from uninitialized Memory Region  (Address " << addr <<")" << endl;
        rs->violationHandler(RuntimeSystem::INVALID_READ,desc.str());
    }
}

void MemoryManager::checkWrite(addr_type addr, size_t size)
{
    RuntimeSystem * rs = RuntimeSystem::instance();

    MemoryType * mt = findContainingMem(addr,size);
    if(!mt)
    {
        stringstream desc;
        desc << "Trying to write to non-allocated MemoryRegion (Address " << addr <<")" << endl;

        MemoryType * possMatch = findPossibleMemMatch(addr);
        if(possMatch)
            desc << "Did you try to write to this region:" << endl << *possMatch << endl;

        rs->violationHandler(RuntimeSystem::INVALID_WRITE,desc.str());
        return;
    }

    assert(addr >= mt->getAddress());
    int from = addr - mt->getAddress();
    mt->initialize(from,from + size);
}


void MemoryManager::checkForNonFreedMem() const
{
    stringstream desc;
    desc << "Program terminated and the following allocations were not freed:" << endl;
    print(desc);
    RuntimeSystem::instance()->violationHandler(RuntimeSystem::MEMORY_LEAK,desc.str());
}


MemoryType * MemoryManager::getMemoryType(addr_type addr)
{
    MemoryType * possibleMatch = findPossibleMemMatch(addr);

    return possibleMatch->getAddress() == addr ? possibleMatch : NULL;
}


void MemoryManager::print(ostream & os) const
{
    if(mem.size() ==0 )
    {
        os << "Memory Manager: No allocations" << endl;
        return;
    }

    os << endl;
    os << "------------------------------- Memory Manager Status -----------------------------" << endl << endl;
    for(set<MemoryType*>::iterator i = mem.begin(); i != mem.end(); ++i)
        os << **i << endl;

    os << "-----------------------------------------------------------------------------------" << endl;
    os << endl;
}

ostream& operator<< (ostream &os, const MemoryManager & m)
{
    m.print(os);
}


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
{
    beginScope("Globals");
}

void RuntimeSystem::violationHandler(Violation v, const string & desc)
{
    ostream & outstr = cerr;

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


void RuntimeSystem::registerFileOpen(FILE * file)
{
    if( openFiles.find(file) != openFiles.end() )
    {


    }
}

void RuntimeSystem::registerFileClose(FILE * file)
{

}

void RuntimeSystem::checkFileAccess(FILE * file)
{

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

    os << "-----------------------------------------------------------------------------------" << endl;
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
