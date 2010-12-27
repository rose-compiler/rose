// vim:sta et ts=4 sw=4:
#include "CppRuntimeSystem.h"
#include <cassert>
#include <sstream>
#include <iomanip>

#include <fstream>

#include <cstdlib> //needed for getenv("HOME") to read config file

#include <boost/foreach.hpp>

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
    : testingMode( false ), qtDebugger(false), defaultOutStr(&cout)
{

    // by default abort on all violations except INVALID_PTR_ASSIGN
    for(int i=0; i< RuntimeViolation::UNKNOWN_VIOLATION; i++)
    {
        RuntimeViolation::Type t = static_cast<RuntimeViolation::Type>(i);
        violationTypePolicy[t] = ViolationPolicy::Exit;
    }
    violationTypePolicy[ RuntimeViolation::INVALID_PTR_ASSIGN ]
        = ViolationPolicy::InvalidatePointer;

    readConfigFile();
}

void RuntimeSystem::printMessage(std::string message) {
#ifdef ROSE_WITH_ROSEQT
  if( isQtDebuggerEnabled() ) {
    RtedDebug::instance()->addMessage(message);
  } else {
    //cerr << "++++++++++++++++++++++++++ " << message << endl;
  }
#else
  cerr << "w/o QT: " << message << endl;
#endif
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
        string value_name;
        lineStream >> key;
        lineStream >> value_name;
        ViolationPolicy::Type value = getPolicyFromString( value_name );

        if(key == "qtDebugger")
            qtDebugger = (
                value_name == "1"
                || value_name == "yes"
                || value_name == "on"
                || value_name == "true"
            );
        else
        {
            RuntimeViolation::Type t = RuntimeViolation::getViolationByString(key);
            if (t == RuntimeViolation::UNKNOWN_VIOLATION)\
                cerr << "Unknown key " << key << endl;
            else if( value == ViolationPolicy::Invalid )
                cerr << "Unknown Policy" << value_name << endl;
            else
                violationTypePolicy[t]=value;
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

ViolationPolicy::Type RuntimeSystem::getPolicyFromString( std::string &name ) const {
    if      ( "Exit"                == name ) return ViolationPolicy::Exit;
    else if ( "Warn"                == name ) return ViolationPolicy::Warn;
    else if ( "Ignore"              == name ) return ViolationPolicy::Ignore;
    else if ( "InvalidatePointer"   == name ) return ViolationPolicy::InvalidatePointer;
    else                                      return ViolationPolicy::Invalid;
}

void RuntimeSystem::setViolationPolicy( RuntimeViolation::Type violation, ViolationPolicy::Type policy ) {
    violationTypePolicy[ violation ] = policy;
}

void RuntimeSystem::violationHandler(RuntimeViolation::Type v, const std::string & description)  throw (RuntimeViolation)
{
    RuntimeViolation vio(v,description);
    violationHandler(vio);
}

void RuntimeSystem::violationHandler(RuntimeViolation & vio)  throw (RuntimeViolation)
{
    if( vio.getType() == RuntimeViolation::NONE )
        return;

    vio.setPosition(curPos);
    ViolationPolicy::Type policy = violationTypePolicy[ vio.getType() ];

    switch( policy ) {
        case ViolationPolicy::Exit:
        case ViolationPolicy::Warn:
            (*defaultOutStr) << vio  << endl;
        default:
            ;// do nothing
    }


#ifdef ROSE_WITH_ROSEQT
    if(qtDebugger)
    {
        //Display the Debugger after violation occured
        stringstream s;
        s << vio;

        if( ViolationPolicy::Exit == policy )
            RtedDebug::instance()->addMessage(s.str().c_str(),RtedDebug::ERROR);
        else
            RtedDebug::instance()->addMessage(s.str().c_str(),RtedDebug::WARNING);

        RtedDebug::instance()->startGui(qtDebugger);
    }
#endif

    if( ViolationPolicy::Exit == policy ) {
        if( testingMode )
            throw vio;
        else
            exit( 0 );
    }

    return;
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


void RuntimeSystem::createMemory(MemoryAddress addr, size_t size, MemoryType::AllocKind kind, RsType * type)
{
  // "Warning: Stack Memory registered without type!"
  assert(kind != MemoryType::StackAlloc || type != NULL);

  // the created MemoryType is freed by memory manager
  const MemoryType mt(addr, size, kind, curPos);
  MemoryType*      nb = memManager.allocateMemory(mt);

  if (kind == MemoryType::StackAlloc && nb != NULL)
  {
    nb->registerMemType(0, type);
    assert(type->getByteSize() == size);
  }
}


void RuntimeSystem::createStackMemory(MemoryAddress addr, size_t size, const std::string& strType)
{
    RsType * type = typeSystem.getTypeInfo(strType);

    createMemory(addr, size, MemoryType::StackAlloc, type);
}



void RuntimeSystem::freeMemory(MemoryAddress startAddress, MemoryType::AllocKind kind)
{
    memManager.freeMemory(startAddress, kind);
}


void RuntimeSystem::checkMemRead(MemoryAddress addr, size_t size, RsType * t)
{
    memManager.checkRead(addr,size,t);
}

void RuntimeSystem::checkMemWrite(MemoryAddress addr, size_t size, RsType * t)
{
    memManager.checkWrite(addr,size,t);
}


// --------------------- Stack Variables ---------------------------------


void RuntimeSystem::createVariable(MemoryAddress address,
                                   const string & name,
                                   const string & mangledName,
                                   const string & typeString)
{
    //TODO deprecated, because with typeString no pointer and arrays can be registered
    TypeSystem* ts = getTypeSystem();
    RsType*     type = ts->getTypeInfo(typeString);

    if(!type)
    {
        if(typeString == "SgArrayType")
        {
            //TODO just until registration is done correct
            cerr << "Trying to register an array via createVariable!" << endl;
            cerr << "Use createArray instead" << endl;
            type = ts->getTypeInfo("SgPointerType");
            // return;
        }
        else
            assert(false);//unknown type
    }

    assert(type != NULL);
    createVariable(address, name, mangledName, type);
}

void RuntimeSystem::createVariable(MemoryAddress address,
                                   const string & name,
                                   const string & mangledName,
                                   RsType *  type)
{
  assert(type);
  stackManager.addVariable(new VariablesType(name,mangledName,type,address));
}




void RuntimeSystem::createArray( MemoryAddress address,
                                 const std::string & name,
                                 const std::string & mangledName,
                                 const std::string & baseType,
                                 size_t size)
{
    RsType * t = typeSystem.getTypeInfo(baseType);
	assert( t );
    createArray(address,name,mangledName,t,size);
}


void RuntimeSystem::createArray(    MemoryAddress address,
                                    const std::string & name,
                                    const std::string & mangledName,
                                    RsType * baseType,
                                    size_t size)
{
    RsArrayType * arrType = typeSystem.getArrayType(baseType,size);

	createArray( address, name, mangledName, arrType);
}


void RuntimeSystem::createArray(    MemoryAddress address,
                                    const std::string & name,
                                    const std::string & mangledName,
                                    RsArrayType * type)
{
  assert(type);
    stackManager.addVariable( new VariablesType( name, mangledName, type, address ));
    pointerManager.createPointer( address, type -> getBaseType() );

    // View an array as a pointer, which is stored at the address and which points at the address
    pointerManager.registerPointerChange( address, address, false, false);
}


void RuntimeSystem::createObject(MemoryAddress address, RsClassType* type )
{
    assert(type != NULL);

    const size_t szObj = type->getByteSize();
    MemoryType*  mt = memManager.findContainingMem(address);

    if (mt != NULL) {
        if (szObj > mt -> getSize() && mt->getAddress() == address) {
            // Same address, larger size.  We assume that a base type was
            // registered, and now its subtype's constructor has been called

            mt -> resize( type -> getByteSize() );
            mt -> forceRegisterMemType( 0, type );
        }

        // \pp what if mt->getAddress() == address?
        return;
    }

    // create a new entry
    const MemoryType newBlock(address, type->getByteSize(), MemoryType::CxxStyleAlloc, curPos);
    MemoryType*      nb = memManager.allocateMemory(newBlock);

    // after the new block is allocated (and if the allocation succeeded)
    //   register the proper memory layout
    if (nb) nb->registerMemType(0, type);
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


void RuntimeSystem::registerPointerChange(MemoryAddress source, MemoryAddress target, bool checkPointerMove, bool checkMemLeaks)
{
    pointerManager.registerPointerChange(source,target,checkPointerMove, checkMemLeaks);
}

void RuntimeSystem::registerPointerChange(MemoryAddress source, MemoryAddress target,RsType * type, bool checkPointerMove, bool checkMemLeaks)
{
    RsPointerType* pt = dynamic_cast<RsPointerType*>( type );
    assert( pt );
    pointerManager.registerPointerChange(source,target,pt -> getBaseType(),checkPointerMove, checkMemLeaks);
}

void RuntimeSystem::registerPointerChange( const std::string & mangledName, MemoryAddress target, bool checkPointerMove, bool checkMemLeaks)
{
    MemoryAddress source = stackManager.getVariable(mangledName)->getAddress();
    pointerManager.registerPointerChange(source,target,checkPointerMove, checkMemLeaks);
}



void RuntimeSystem::checkPointerDereference( MemoryAddress source, MemoryAddress derefed_address )
{
    pointerManager.checkPointerDereference(source,derefed_address);
}

void RuntimeSystem::checkIfThisisNULL(void* thisExp) {

	pointerManager.checkIfPointerNULL(thisExp);
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




void RuntimeSystem::registerFileOpen(std::fstream& file,const string & openendFile, int mode)
{
      fileManager.openFile(file, openendFile,(FileOpenMode)mode, curPos);
}

void RuntimeSystem::registerFileOpen(std::fstream& file,const string & openendFile, const string & mode_str)
{
    fileManager.openFile(file, openendFile,mode_str, curPos);
}

void RuntimeSystem::registerFileClose(std::fstream& file)
{
    fileManager.closeFile(file);
}

void RuntimeSystem::checkFileAccess(std::fstream& file, bool read)
{
   fileManager.checkFileAccess(file,read);
}

// --------------------- Function Signature Verification -----------------

void RuntimeSystem::expectFunctionSignature(
            const std::string & name,
            const std::vector< RsType* > types ) {

    nextCallFunctionName = name;
    nextCallFunctionTypes = types;
}

void RuntimeSystem::confirmFunctionSignature(
            const std::string & name,
            const std::vector< RsType* > types ) {

    if( !( name == nextCallFunctionName ))
        // nothing to confirm
        return;

    if( nextCallFunctionTypes != types ) {
        stringstream ss;
        ss  << "A call to function " << name
            << " had unexpected return or parameter type(s)." << endl << endl

            << "The callsite expected the following:" << endl;
        BOOST_FOREACH( RsType* type, nextCallFunctionTypes ) {
            ss << "\t" << type -> getDisplayName() << endl;
        }

        ss  << "But the definition is:" << endl;
        BOOST_FOREACH( RsType* type, types ) {
            ss << "\t" << type -> getDisplayName() << endl;
        }

        violationHandler(
            RuntimeViolation::UNEXPECTED_FUNCTION_SIGNATURE,
            ss.str() );
    }
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
