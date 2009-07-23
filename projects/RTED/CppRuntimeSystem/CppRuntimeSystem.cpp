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


void RuntimeSystem::createMemory(addr_type addr, size_t size, bool onStack, RsType * type)
{
    // the created MemoryType is freed by memory manager
    MemoryType * mt = new MemoryType(addr,size,curPos,onStack);

    if(onStack)
    {
        if(type)
        {
            mt->accessMemWithType(0,type);
            assert(type->getByteSize() == size);
        }
        else //TODO make error to assert, if registration is done correct
            cerr << "Warning: Stack Memory registered without type!" << endl;
    }

    memManager.allocateMemory(mt);
}

void RuntimeSystem::createStackMemory(addr_type addr, size_t size,const std::string & strType)
{
    MemoryType * mt = new MemoryType(addr,size,curPos,true);

    RsType * type = typeSystem.getTypeInfo(strType);
    assert(type);

    mt->accessMemWithType(0,type);

    memManager.allocateMemory(mt);
}



void RuntimeSystem::freeMemory(addr_type startAddress, bool onStack)
{
    memManager.freeMemory(startAddress, onStack);
}


void RuntimeSystem::checkMemRead(addr_type addr, size_t size, RsType * t)
{
    memManager.checkRead(addr,size,t);
}

void RuntimeSystem::checkMemWrite(addr_type addr, size_t size, RsType * t)
{
    memManager.checkWrite(addr,size,t);
}


// --------------------- Stack Variables ---------------------------------


void RuntimeSystem::createVariable(addr_type address,
                                   const string & name,
                                   const string & mangledName,
                                   const string & typeString)
{
    stackManager.addVariable(new VariablesType(name,mangledName,typeString,address));
}

void RuntimeSystem::createVariable(addr_type address,
                                   const string & name,
                                   const string & mangledName,
                                   RsType *  type)
{
    stackManager.addVariable(new VariablesType(name,mangledName,type,address));
}


/// Convenience function which also calls createPointer
void RuntimeSystem::createVariable( addr_type address,
                                    const string & name,
                                    const string & mangledName,
                                    const string & typeString,
                                    const string & pointerType)
{
    stackManager.addVariable(new VariablesType(name,mangledName,typeString,address));
    if(pointerType.size() > 0)
        createPointer(address,pointerType);
}

void RuntimeSystem::createArray( addr_type address,
                                 const std::string & name,
                                 const std::string & mangledName,
                                 const std::string & baseType,
                                 size_t size)
{
    RsType * t = typeSystem.getTypeInfo(baseType);
    createArray(address,name,mangledName,t,size);
}


void RuntimeSystem::createArray(    addr_type address,
                                    const std::string & name,
                                    const std::string & mangledName,
                                    RsType * baseType,
                                    size_t size)
{
    RsType * arrType = typeSystem.getArrayType(baseType,size);

    stackManager.addVariable(new VariablesType(name,mangledName,arrType,address));
    pointerManager.createPointer(address, baseType);
    // View an array as a pointer, which is stored at the address and which points at the address
    pointerManager.registerPointerChange(address,address,false);
}





void RuntimeSystem::beginScope(const std::string & name)
{
    stackManager.beginScope(name);
}

void RuntimeSystem::endScope ()
{
    stackManager.endScope();
}


// --------------------- Pointer Tracking---------------------------------

void RuntimeSystem::createPointer(addr_type sourceAddr, RsType * type)
{
    pointerManager.createPointer(sourceAddr,type);
}

void RuntimeSystem::createPointer(addr_type sourceAddr, const string & typeStr)
{
    RsType * type = typeSystem.getTypeInfo(typeStr);
    if(!type)    //TODO make this as assert
    {
        cerr << "SEVERE WARNING: Called createPointer with unknown type " << typeStr << endl;
        cerr << "Pointer is not created" << endl;
        return;
    }


    pointerManager.createPointer(sourceAddr,type);
}


void RuntimeSystem::registerPointerChange(addr_type source, addr_type target, bool checks)
{
    pointerManager.registerPointerChange(source,target,checks);
}

void RuntimeSystem::registerPointerChange( const std::string & mangledName, addr_type target, bool checks)
{
    addr_type source = stackManager.getVariable(mangledName)->getAddress();
    pointerManager.registerPointerChange(source,target,checks);
}



void RuntimeSystem::checkPointerDereference( addr_type source, addr_type derefed_address )
{
    pointerManager.checkPointerDereference(source,derefed_address);
}


// --------------------- File Management ---------------------------------

void RuntimeSystem::registerFileOpen(FILE * file,const string & openendFile, int mode)
{
    fileManager.openFile(file, openendFile,(FileOpenMode)mode, curPos);
}

void RuntimeSystem::registerFileOpen(FILE * file,const string & openendFile, const string & mode_str)
{
    fileManager.openFile(file, openendFile,mode_str, curPos);
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
    stackManager.endScope();
    // allows you to call doProgramExitChecks but then keep going without first
    // calling clearStatus.  Convenient for testing, but not generally
    // recommended.
    stackManager.beginScope("Globals");


    // Check for memory leaks
    memManager.checkForNonFreedMem();
    fileManager.checkForOpenFiles();
}


void RuntimeSystem::clearStatus()
{
    memManager.clearStatus();
    fileManager.clearStatus();
    typeSystem.clearStatus();
    stackManager.clearStatus();
    pointerManager.clearStatus();

    curPos = SourcePosition();
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






