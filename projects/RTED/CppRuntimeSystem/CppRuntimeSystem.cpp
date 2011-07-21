// vim:sta et ts=4 sw=4:
#include <cassert>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib> //needed for getenv("HOME") to read config file

#include <boost/foreach.hpp>

#include "CppRuntimeSystem.h"

#ifdef ROSE_WITH_ROSEQT
#include "DebuggerQt/RtedDebug.h"
#endif

#include "rtedsync.h"


using namespace std;

RuntimeSystem * RuntimeSystem::single = NULL;

RuntimeSystem* RuntimeSystem::instance()
{
    if (!single)
    {
        single = new RuntimeSystem();
        single->registerOutputStream(&std::cerr);
    }

    return single;
}

std::ostream& RuntimeSystem::log() { return std::cout; }

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

void RuntimeSystem::printMessage(const std::string& message) {
#ifdef ROSE_WITH_ROSEQT
  if( isQtDebuggerEnabled() ) {
    RtedDebug::instance()->addMessage(message);
  }
#else
  std::stringstream msg;

  msg << "trace (#" << rted_ThisThread() << "): "
      << message << '\n';

  this->log() << msg.str() << std::flush;
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


void RuntimeSystem::checkpoint(const SourcePosition& pos)
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
    std::stringstream     errmsg;

    errmsg << vio << " Thread# " << rted_ThisThread() << '\n';

    if (policy == ViolationPolicy::Exit || policy == ViolationPolicy::Warn)
    {
      rted_UpcBeginExclusive();
      (*defaultOutStr) << errmsg.str() << std::flush;
      rted_UpcEndExclusive();
    }


#ifdef ROSE_WITH_ROSEQT
    if(qtDebugger)
    {
        //Display the Debugger after violation occured

        if ( ViolationPolicy::Exit == policy )
            RtedDebug::instance()->addMessage(errmsg.str(), RtedDebug::ERROR);
        else
            RtedDebug::instance()->addMessage(errmsg.str(), RtedDebug::WARNING);

        RtedDebug::instance()->startGui(qtDebugger);
    }
#endif

    if ( policy != ViolationPolicy::Exit ) return;

    if ( testingMode ) throw vio;

    rted_exit( 0 );
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


void RuntimeSystem::createMemory(Address addr, size_t size, MemoryType::AllocKind kind, long blocksize, const RsType* type)
{
  // \pp it seems that we only get the type for stack allocations
  //     should the type initialization then be moved to createStackMemory?
  //     why do not we get types for C++ new?
  //     is alloca handled?

  // \pp \todo move the registerMemType call into allocateMemory
  MemoryType* nb = memManager.allocateMemory(addr, size, kind, blocksize, curPos);

  if ((kind & (akStack | akGlobal)) && nb != NULL)
  {
    // stack memory must have a type
    assert(type != NULL);

    nb->registerMemType(addr, 0, type);
    assert(type->getByteSize() == size);
  }
}


void RuntimeSystem::createStackMemory(Address addr, size_t size, const std::string& strType)
{
    // \note only called once (by test.cpp)
    const RsType* type = typeSystem.getTypeInfo(strType);
    const long    nondistributed = 0;

    createMemory(addr, size, akStack, nondistributed, type);
}


void RuntimeSystem::freeMemory(Address addr, MemoryType::AllocKind kind)
{
    memManager.freeHeapMemory(addr, kind);
}


void RuntimeSystem::checkMemRead(Address addr, size_t size) const
{
    memManager.checkRead(addr, size);
}

void RuntimeSystem::checkMemLoc(Address addr, size_t size) const
{
    // \todo the violation should become invalid access
    memManager.checkLocation(addr, size, RuntimeViolation::INVALID_READ);
}


        /// Checks if the specified memory region is allocated
void RuntimeSystem::checkBounds(Address addr, Address accaddr, size_t size) const
{
    memManager.checkIfSameChunk(addr, accaddr, size);
}

bool
RuntimeSystem::checkMemWrite(Address addr, size_t size, const RsType* t)
{
    std::pair<MemoryType*, int>  res = memManager.checkWrite(addr, size, t);
    AllocKind                    ak = res.first->howCreated();

    if ((ak & akUpcShared) == 0) return 0;
    return res.second;
}


// --------------------- Stack Variables ---------------------------------


void RuntimeSystem::createVariable( Address            address,
                                    const std::string& name,
                                    const std::string& mangledName,
                                    const std::string& typeString,
                                    AllocKind          ak,
                                    long               blocksize
                                  )
{
    //TODO deprecated, because with typeString no pointer and arrays can be registered
    TypeSystem*   ts = getTypeSystem();
    const RsType* type = ts->getTypeInfo(typeString);

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
    createVariable(address, name, mangledName, type, ak, blocksize);
}

void RuntimeSystem::createVariable( Address            address,
                                    const std::string& name,
                                    const std::string& mangledName,
                                    const RsType*      type,
                                    AllocKind          ak,
                                    long               blocksize
                                  )
{
  assert( type && (ak & (akStack | akGlobal)) );

  // \todo remove the dynamic cast by overloading the interface
  const RsClassType* class_type = dynamic_cast< const RsClassType* >( type );

  // Variables are allocated statically. However, in UPC they can be shared
  //   if allocated on the file scope. Thus, the locality holds only for
  //   non UPC types (e.g., C++ classes).
  assert(class_type == NULL || rted_isLocal(address));

  MemoryType*        mem = class_type ? memManager.getMemoryType(address) : NULL;
  const bool         isCtorCall = (mem != NULL);

  // When we create classes, the memory might be allocated in the
  // constructor.  In these cases, it's fine to call createvar with
  // existing memory
  if (isCtorCall)
  {
    mem->fixAllocationKind(ak);
  }
  else
  {
    createMemory(address, type->getByteSize(), ak, blocksize, type);
  }

  // \pp \note it seems that addVariable again registers all pointers, that
  //           have already been registered by createMemory
  stackManager.addVariable(new VariablesType(address, name, mangledName, type), blocksize);
}




void RuntimeSystem::createArray( Address address,
                                 const std::string & name,
                                 const std::string & mangledName,
                                 const std::string & baseType,
                                 size_t size,
                                 AllocKind ak,
                                 long blocksize
                               )
{
  const RsType* t = typeSystem.getTypeInfo(baseType);
  assert( t );
  createArray(address, name, mangledName, t, size, ak, blocksize);
}


void RuntimeSystem::createArray( Address address,
                                 const std::string & name,
                                 const std::string & mangledName,
                                 const RsType* baseType,
                                 size_t size,
                                 AllocKind ak,
                                 long blocksize
                               )
{
  const RsArrayType* arrType = typeSystem.getArrayType(baseType, size);

  createArray( address, name, mangledName, arrType, ak, blocksize );
}


void RuntimeSystem::createArray( Address address,
                                 const std::string& name,
                                 const std::string& mangledName,
                                 const RsArrayType* type,
                                 AllocKind ak,
                                 long blocksize
                               )
{
  assert(type);

  // creates array on the stack
  createVariable( address, name, mangledName, type, ak, blocksize );
  pointerManager.createPointer( address, type->getBaseType(), blocksize );

  // View an array as a pointer, which is stored at the address and which points at the address
  pointerManager.registerPointerChange( address, address, false, false );
}


void RuntimeSystem::createObject(Address address, const RsClassType* type)
{
    assert(type != NULL);

    const size_t         szObj = type->getByteSize();
    MemoryType*          mt = memManager.findContainingMem(address, 1);

    if (mt)
    {
        const bool base_ctor = (szObj > mt->getSize()) && (address == mt->beginAddress());

        if (base_ctor)
        {
            // Same address, larger size.  We assume that a base type was
            // registered, and now the derived's constructor has been called
            mt -> resize( szObj );

            // \note address == mt->beginAddress(), thus ofs of forceRegisterType is always 0
            mt -> forceRegisterMemType( type );
        }

        // \pp \todo what if !base_ctor?
        return;
    }

    // create a new entry
    const long  nondistributed = 0; // C++ only operates on non-distribited space

    // \note we assume that this object is created on the heap ...
    //       for stack objects this information is fixed later
    //       see MemoryType::fixAllocationKind
    MemoryType* nb = memManager.allocateMemory(address, szObj, akCxxNew, nondistributed, curPos);

    // after the new block is allocated (and if the allocation succeeded)
    //   register the proper memory layout
    if (nb) nb->registerMemType(address, 0, type);
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


void RuntimeSystem::registerPointerChange( Address     src,
                                           Address     tgt,
                                           bool        checkPointerMove,
                                           bool        checkMemLeaks
                                         )
{
    pointerManager.registerPointerChange(src, tgt, checkPointerMove, checkMemLeaks);
}

void RuntimeSystem::registerPointerChange( Address              src,
                                           Address              tgt,
                                           const RsPointerType* pt,
                                           bool                 checkPointerMove,
                                           bool                 checkMemLeaks
                                         )
{
    assert( pt );

    pointerManager.registerPointerChange(src, tgt, pt->getBaseType(), checkPointerMove, checkMemLeaks);
}


void RuntimeSystem::checkIfThisisNULL(void* thisExp) const
{
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

void RuntimeSystem::expectFunctionSignature(std::string name, TypeList& types)
{
    nextCallFunctionName.swap(name);
    nextCallFunctionTypes.swap(types);
}

void RuntimeSystem::confirmFunctionSignature(const std::string& name, const TypeList& types) const
{
    // nothing to confirm
    if (!( name == nextCallFunctionName )) return;

    if ( nextCallFunctionTypes != types )
    {
        stringstream ss;
        ss  << "A call to function " << name
            << " had unexpected return or parameter type(s)." << endl << endl

            << "The callsite expected the following:" << endl;
        BOOST_FOREACH( const RsType* type, nextCallFunctionTypes ) {
            ss << "\t" << type -> getDisplayName() << endl;
        }

        ss  << "But the definition is:" << endl;
        BOOST_FOREACH( const RsType* type, types ) {
            ss << "\t" << type -> getDisplayName() << endl;
        }

        RuntimeSystem::instance()->violationHandler( RuntimeViolation::UNEXPECTED_FUNCTION_SIGNATURE, ss.str() );
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


/// status variable
int diagnostics::status = 0; /* diagnostics::memory | diagnostics::location; */

#if OBSOLETE_CODE
// \pp \todo remove superfluous parameter
void RuntimeSystem::checkPointerDereference(Address src, AddressDesc, Address derefed_address, AddressDesc)
{
    pointerManager.checkPointerDereference(src, derefed_address);
}
#endif /* OBSOLETE_CODE */
