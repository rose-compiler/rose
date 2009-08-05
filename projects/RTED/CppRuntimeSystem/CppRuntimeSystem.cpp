#include "CppRuntimeSystem.h"
#include <cassert>
#include <sstream>
#include <iomanip>

#include <fstream>

#include <cstdlib> //needed for getenv("HOME") to read config file

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

    // by default abort on all violations except INVALID_PTR_ASSIGN
    for(int i=0; i< RuntimeViolation::UNKNOWN_VIOLATION; i++)
    {
        RuntimeViolation::Type t = static_cast<RuntimeViolation::Type>(i);
        vioAbortInfoMap[t]=true;
    }
    vioAbortInfoMap[RuntimeViolation::INVALID_PTR_ASSIGN] = false;

    readConfigFile();
}


void RuntimeSystem::readConfigFile()
{
    ifstream file;
    file.open("RTED.cfg",ifstream::in);

    if(!file)
    {
        string path( getenv("HOME") );
        path += "/RTED.cfg";
        file.open(path.c_str(),ifstream::in);
    }
    if(!file)
    {
        cerr << "No RTED.cfg found" << endl;
        return;
    }

    string line;
    while( getline(file,line))
    {
        stringstream lineStream(line);
        string key;
        bool value;
        lineStream >> key;
        lineStream >> value;
        if(key == "qtDebugger")
            qtDebugger=value;
        else
        {
            RuntimeViolation::Type t = RuntimeViolation::getViolationByString(key);
            if (t == RuntimeViolation::UNKNOWN_VIOLATION)\
                cerr << "Unknown key " << key << endl;
            else
                vioAbortInfoMap[t]=value;
        }
    }

    file.close();
}


void RuntimeSystem::checkpoint(const SourcePosition & pos)
{
    curPos = pos;

#ifdef ROSE_WITH_ROSEQT
    if(qtDebugger)
        RtedDebug::instance()->startGui();
#endif
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

    // vioAbortInfoMap maps types to bool, giving us two policies, 
    //      { ignore, exit }
    // it would be good to add a third policy (warn).  Doing so would require
    // modifying either the makefile or sed scripts to ensure that the expected
    // and actual errors still matched.
    if( vioAbortInfoMap[ vio.getType() ])
        (*defaultOutStr) << vio  << endl;




#ifdef ROSE_WITH_ROSEQT
    if(qtDebugger)
    {
        //Display the Debugger after violation occured
        stringstream s;
        s << vio;

        if(vioAbortInfoMap[vio.getType()])
            RtedDebug::instance()->addMessage(s.str().c_str(),RtedDebug::ERROR);
        else
            RtedDebug::instance()->addMessage(s.str().c_str(),RtedDebug::WARNING);

        RtedDebug::instance()->startGui(qtDebugger);
    }
#endif

    // check if we ought to abort on this type of violation
    // testing mode aborts on all violations
    if(! testingMode && !vioAbortInfoMap[vio.getType()])
        return;


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
    //TODO deprecated, because with typeString no pointer and arrays can be registered
    stackManager.addVariable(new VariablesType(name,mangledName,typeString,address));
}

void RuntimeSystem::createVariable(addr_type address,
                                   const string & name,
                                   const string & mangledName,
                                   RsType *  type)
{
    stackManager.addVariable(new VariablesType(name,mangledName,type,address));
}




void RuntimeSystem::createArray( addr_type address,
                                 const std::string & name,
                                 const std::string & mangledName,
                                 const std::string & baseType,
                                 size_t size)
{
    RsType * t = typeSystem.getTypeInfo(baseType);
	assert( t );
    createArray(address,name,mangledName,t,size);
}


void RuntimeSystem::createArray(    addr_type address,
                                    const std::string & name,
                                    const std::string & mangledName,
                                    RsType * baseType,
                                    size_t size)
{
    RsArrayType * arrType = typeSystem.getArrayType(baseType,size);

	createArray( address, name, mangledName, arrType);
}


void RuntimeSystem::createArray(    addr_type address,
                                    const std::string & name,
                                    const std::string & mangledName,
                                    RsArrayType * type)
{
    stackManager.addVariable( new VariablesType( name, mangledName, type, address ));
    pointerManager.createPointer( address, type -> getBaseType() );

    // View an array as a pointer, which is stored at the address and which points at the address
    pointerManager.registerPointerChange( address, address, false );
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






