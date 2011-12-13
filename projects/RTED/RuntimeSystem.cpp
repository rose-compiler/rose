// vim:sw=2 ts=2 et sta:
/*
 * RuntimeSystem.cpp
 *
 *  Created on: Jul 2, 2009
 *      Author: panas2
 */

#include <stdarg.h>

#include <cassert>
#include <iostream>
#include <string>
#include <bitset>
#include <vector>
#include <algorithm>
#include <numeric>

#include "RuntimeSystem.h"
#include "ParallelRTS.h"

#include "CppRuntimeSystem/CppRuntimeSystem.h"

#ifdef ROSE_WITH_ROSEQT
#include "CppRuntimeSystem/DebuggerQt/RtedDebug.h"
#endif

enum ReadWriteMask { Read = 1, Write = 2, BoundsCheck = 4 };

static const int primaryLoc = 1;

static unsigned long ctrCreateVar = 0;
static unsigned long ctrInitVar = 0;
static unsigned long ctrAccessVar = 0;
static unsigned long ctrAccessArray = 0;
static unsigned long ctrEnterScope = 0;
static unsigned long ctrExitScope = 0;

#ifdef RTED_STATS
static const bool with_stats = true;
#else
static const bool with_stats = false;
#endif /* RTED_STATS */

static inline
void stats_incr(unsigned long& ctr)
{
  if (with_stats) ++ctr;
}

void stats_report(RuntimeSystem& rs)
{
  if (!with_stats) return;

  std::stringstream out;

  out << "@ CreateVar: " << ctrCreateVar
      << "  InitVar: " << ctrInitVar
      << "  AccessVar: " << ctrAccessVar
      << "  AccessArray: " << ctrAccessArray
      << "  EnterScope/ExitScope: " << ctrEnterScope << "/" << ctrExitScope
      << std::endl;

  rted_UpcBeginExclusive();
  rs.printMessage(out.str());
  rted_UpcEndExclusive();
}


static inline
void checkpoint(RuntimeSystem& rs, const SourceInfo& info, bool primaryorigin)
{
  rs.checkpoint(info);

  if (diagnostics::message(diagnostics::location))
  {
    std::stringstream msg;
    msg << "at line " << info.src_line << " / " << info.rted_line;

    if (!primaryorigin) msg << " *upcmsg*";

    msg << "\n";

    rs.printMessage(msg.str());
  }
}

/*********************************************************
 * This function is closed when RTED finishes (Destructor)
 ********************************************************/
void rted_Close(const char*)
{
  RuntimeSystem& rs = RuntimeSystem::instance();

  stats_report(rs);

  // wait until all threads have finished their work
  rted_barrier();

  // process the last messages
  rted_ProcessMsg();

  // perform exit checks
  rs.doProgramExitChecks();
  rs.printMessage("ROSE-RTED did not find an error.\n");

  rted_UpcExitWorkzone();
}


// ***************************************** ARRAY FUNCTIONS *************************************

struct ArrayTypeComputer
{
  typedef std::pair<const RsArrayType*, size_t> Operand;

  TypeSystem& ts;

  explicit
  ArrayTypeComputer(TypeSystem& tsys)
  : ts(tsys)
  {}

  Operand operator()(Operand prev, size_t dimSize)
  {
    size_t newsz = prev.second * dimSize;

    return Operand(ts.getArrayType(prev.first, newsz), newsz);
  }
};


/// \brief Returns an RsArrayType* whose dimensionality dimensions are given by dimDesc and
///        whose base non-array type is base_type.
static
const RsArrayType*
rs_getArrayType(TypeSystem& ts, const size_t* dimDesc, size_t size, const RsType& base_type)
{
  assert( dimDesc != NULL && *dimDesc > 0 );

  // collect dimension information
  const size_t*      first = dimDesc + 1;
  const size_t*      last = dimDesc + *dimDesc;  // points to the last element (not one past!)
                                                //   which is the first element being processed
  size_t             no_elems = std::accumulate(first, last+1, static_cast<size_t>(1), std::multiplies<size_t>());
  size_t             elem_size = (size / no_elems);
  size_t             last_sz = elem_size * (*last);
  const RsArrayType* res = ts.getArrayType( &base_type, last_sz );

  return std::accumulate( std::reverse_iterator<const size_t*>(last),
                          std::reverse_iterator<const size_t*>(first),
                          ArrayTypeComputer::Operand(res, last_sz),
                          ArrayTypeComputer(ts)
                        ).first;
}

static
const RsType* rs_getTypeInfo(const TypeSystem& ts, const std::string& type)
{
  static const RsType* cached = ts.getTypeInfo("SgTypeInt");

  const RsType* result = NULL;

  if (cached->getName() == type)
  {
    result = cached;
  }
  else
  {
    result = ts.getTypeInfo(type);

    if (result) cached = result;
  }

  return result;
}


static
const RsType& rs_getTypeInfo_fallback(TypeSystem& ts, const std::string& type, size_t sz)
{
  const RsType* res = rs_getTypeInfo(ts, type);

  if (res == NULL)
  {
    res = &ts.getClassType(type, sz);
  }

  assert(res != NULL);
  return *res;
}

static
std::pair<const RsType*, const RsType*>
rs_getType(TypeSystem& ts, std::string type, std::string base_type, std::string class_name, AddressDesc desc, size_t sz )
{
  typedef std::pair<const RsType*, const RsType*> ResultType;

  if( type == "SgClassType" )
    type = class_name;
  else if( base_type == "SgClassType" )
  {
    base_type = class_name;
    assert( base_type != "" );
  }
  assert( type != "" );

  ResultType res(NULL, NULL);

  if( type == "SgPointerType" )
  {
    assert( desc.levels > 0 );

    res.second = &rs_getTypeInfo_fallback(ts, base_type, sz);
    res.first = ts.getPointerType(res.second, desc);
  }
  else
  {
    res.first = res.second = &rs_getTypeInfo_fallback(ts, type, sz);
  }

  assert(res.first && res.second);
  return res;
}

static
const RsType&
rs_simpleGetType(TypeSystem& ts, std::string type, std::string base_type, std::string class_name, AddressDesc desc, size_t sz)
{
  return *(rs_getType(ts, type, base_type, class_name, desc, sz).first);
}


static
const RsType&
rs_getArrayElemType(TypeSystem& ts, TypeDesc td, size_t noDimensions, const std::string& classname)
{
  assert(td.desc.levels >= noDimensions);

  AddressDesc desc = td.desc;
  std::string elemtype = td.base;

  if (td.desc.levels > noDimensions)
  {
    elemtype = "SgPointerType";
    desc = rted_deref_desc(desc);
  }

  const size_t typesz = 0; // \todo calculate the proper size, if this is about a class type

  return rs_simpleGetType(ts, elemtype, td.base, classname, desc, typesz);
}



/// \brief This function is called when a *variable* with an array type is created
///        (the shared memory version was copied form rted_CreateHeap)
/// \param td          the type description
/// \param address     the base address of the array
/// \param totalsize   the size in bytes of the array
/// \param allocKind   describes the allocation method
///                    either global (possibly shared) or local
/// \param blocksize   UPC blocksize for global shared arrays
/// \param initialized true, iff the elements were initialized
/// \param dimDescr    an array describing the number of dimensions and
///                    the array bounds. e.g., {2, 4, 3} is an array[3][4].
/// \param name        variable name
/// \param mangl_name  unique name
/// \param class_name  name of the underlying type
/// \param si          source location
void rted_CreateArray( rted_TypeDesc   td,
                       rted_Address    address,
                       size_t          totalsize,
                       rted_AllocKind  allocKind,
                       long            blocksize,
                       int             initialized,
                       const size_t*   dimDescr,
                       const char*     name,
                       const char*     mangl_name,
                       const char*     class_name,
                       rted_SourceInfo si
                     )
{
  assert(std::string("SgArrayType") == td.name && dimDescr && *dimDescr > 0);

  RuntimeSystem&     rs = RuntimeSystem::instance();
  checkpoint( rs, si, primaryLoc );

  TypeSystem&        typesys  = rs.getTypeSystem();
  const RsType&      rsbase   = rs_getArrayElemType(typesys, td, *dimDescr, class_name);
  const RsArrayType* rstype   = rs_getArrayType(typesys, dimDescr, totalsize, rsbase);
  const long         blocksz  = rsbase.getByteSize() * blocksize;

  rs.createArray( address, name, mangl_name, rstype, allocKind, blocksz );

  if (initialized) {
    rs.checkMemWrite( address, totalsize );
  }
}



void _rted_AllocMem( rted_TypeDesc    td,
                     rted_Address     address,
                     rted_Address     heap_address,
                     long             blocksize,
                     size_t           mallocSize,
                     rted_AllocKind   allocKind,
                     const char*      class_name,
                     rted_SourceInfo  si,
                     int              originloc
                   )
{
  typedef std::pair<const RsType*, const RsType*> TypeDescriptor;

  assert(std::string("SgPointerType") == td.name && (allocKind & ~akNamedMemory));

  RuntimeSystem&  rs = RuntimeSystem::instance();
  checkpoint( rs, si, originloc );

  std::string base_type(td.base);
  if( base_type == "SgClassType" )
    base_type = class_name;

  TypeDescriptor       rs_type = rs_getType(rs.getTypeSystem(), td.name, td.base, class_name, td.desc, mallocSize);
  const RsPointerType* rs_ptr_type = static_cast< const RsPointerType* >(rs_type.first);
  const RsClassType*   class_type = dynamic_cast< const RsClassType* >(rs_ptr_type->getBaseType());

  if ( diagnostics::message(diagnostics::memory) )
  {
    std::stringstream msg;

    msg << " registering heap type:" << td.name
        << "   basetype: " << td.base
        << "   class_name: " << class_name
        << "   indirection_level: " << ToString(td.desc.levels)
        << "   address: " << heap_address
        << "   malloc size: " << mallocSize
        << std::endl;

    rs.printMessage(msg.str());
  }

  // A class might have had its memory allocation registered in the
  // constructor.  If there was no explicit constructor, however, we still
  // have to allocate the memory here.
  if(!class_type || rs.getMemManager().findContainingMem(heap_address, 1) == NULL)
  {
    // assert(allocKind & ~akNamedMemory);

    // \todo implement blocksize == -2 (automatic UPC blocksize)
    //       low priority b/c the frontend calculates this for us
    assert( blocksize >= 0 );

    const long blocksz = rs_type.second->getByteSize() * blocksize;

    rs.createMemory( heap_address, mallocSize, allocKind, blocksz, NULL );
  }

  rs.registerPointerChange(
      address,
      heap_address,
      *rs_ptr_type,
      false  // checkPtrMove? no, pointer may change regions
      );
}

void rted_AllocMem( rted_TypeDesc   td,
                    rted_Address    address,
                    size_t          /*size*/,
                    rted_AllocKind  allocKind,
                    long            blocksize,
                    size_t          mallocSize,
                    const char*     class_name,
                    rted_SourceInfo si
                  )
{
  rted_ProcessMsg();

  // \note the calculation of heap_address and heap_desc has to be
  //       carried out by the thread performing the operation;
  //       Only the local thread can safely deref local pointers.
  //       if we can discern all pointers on the shared heap, we might
  //       move this code back into _rted_CreateHeapPtr
  Address     heap_address = rted_deref(address, td.desc);

  snd_AllocMem(td, address, heap_address, blocksize, mallocSize, allocKind, class_name, si);
  _rted_AllocMem(td, address, heap_address, blocksize, mallocSize, allocKind, class_name, si, primaryLoc);
}


/*********************************************************
 * This function is called when an array is accessed
 * name      : variable name
 * posA      : array[posA][]
 * posB      : array[][posB]
 * filename  : file location
 * line      : linenumber
 * stmtStr   : unparsed version of the line to be used for error message
 ********************************************************/
void rted_AccessArray( rted_Address     base_address, // &( array[ 0 ])
                       rted_Address     address,
                       size_t           size,
                       int              read_write_mask,  // 1 = read, 2 = write
                       rted_SourceInfo  si
                     )
{
  // \pp writes are handled in rted_InitVariable
  assert( (read_write_mask & Write) == 0 );

  rted_ProcessMsg();
  stats_incr(ctrAccessArray);

  RuntimeSystem&       rs = RuntimeSystem::instance();

  checkpoint( rs, si, primaryLoc );

  if ( read_write_mask & Read )        rs.checkMemRead(address, size);
  if ( read_write_mask & BoundsCheck ) rs.checkBounds(base_address, address, size);
}

// ***************************************** FUNCTION CALL *************************************

struct AsRsType
{
  TypeSystem& ts;

  explicit
  AsRsType(TypeSystem& typesys)
  : ts(typesys)
  {}

  const RsType* operator()(const TypeDesc& td)
  {
    const size_t typesz = 0; // \todo calculate/pass proper size

    return &rs_simpleGetType(ts, td.name, td.base, "", td.desc, typesz);
  }
};

void rted_AssertFunctionSignature( const char* name,
                                   size_t type_count,
                                   TypeDesc* typedescs,
                                   SourceInfo si
                                 )
{
  RuntimeSystem& rs = RuntimeSystem::instance();

  checkpoint( rs, si, primaryLoc );

  RuntimeSystem::TypeList types;

  types.reserve(type_count);
  std::transform(typedescs, typedescs + type_count, std::back_inserter(types), AsRsType(rs.getTypeSystem()));
  rs.expectFunctionSignature(name, types);
}

void rted_ConfirmFunctionSignature(const char* name, size_t type_count, TypeDesc* typedescs)
{
  RuntimeSystem& rs = RuntimeSystem::instance();

  RuntimeSystem::TypeList types;

  types.reserve(type_count);
  std::transform(typedescs, typedescs + type_count, std::back_inserter(types), AsRsType(rs.getTypeSystem()));
  rs.confirmFunctionSignature( name, types );
}


static
void RuntimeSystem_ensure_allocated_and_initialized( Address addr, size_t size)
{
  RuntimeSystem&         rs = RuntimeSystem::instance();

  // We trust that anything allocated is properly initialized -- we're not
  // working around a string constant so there's no need for us to do anything.
  if (rs.getMemManager().findContainingMem(addr, 1) != NULL)
  {
    // \pp \todo should not we at least check that we can write size bytes?
    return;
  }

  const bool nondistributed = false;

  // \pp first we create an array, and then we check that we can write to it
  //     why???
  rs.createArray(addr, "StringConstant", "MangledStringConstant", "SgTypeChar", size, akGlobal, nondistributed);
  rs.checkMemWrite(addr, size);
}


static inline
long numval(const char* str)
{
  return strtol(str, NULL, 10);
}


// This handles string constants.  The present instrumentation does not
// transform these in any way, so the memory manager doesn't know that it's
// legal to read from them, since they're initialized at load time.
//
// A general approach to solving this problem would be to update the
// instrumentation (e.g. by making string constants globals, or otherwise
// registering them during the preamble inserted into main), but in the
// meantime, simply treating them as "lazily initialized" variables is
// sufficient for the memory manager to recognize that reads are valid.
//
// This does, however, mean that the memory manager will also think writes are
// valid, as in, e.g.
//
//    char* s = malloc( 5);
//    strcpy( s, "1234";
//    strcpy( "a constant string", s);
static inline
void handle_string_constant(const char** args, size_t i)
{
  if (isdigit(args[ i + 1 ][0]))
  {
    Address addr = rted_Addr((char*) args[i]);

    RuntimeSystem_ensure_allocated_and_initialized(addr, numval(args[ i + 1 ]));
  }
}

static inline
int num_arg(const char** args, size_t noArgs, size_t idx)
{
  assert (idx < noArgs );

  // \pp ???
  // was: if (i < noArgs - 1 && isdigit( args[ i + 1 ][ 0 ])) ++i;

  return numval(args[idx]);
}


/*********************************************************
 * This function is called when one of the following functions in the code is called:
 * memcpy, memmove, strcpy, strncpy, strcat, strncat
 * fname     : function name that is being called
 * args      : arguments to that function
 *           : format : arg1 as string, arg1 as var name (if applicable),
 *           :          arg2 as string, arg2 as var name (if applicable),
 *           :          number of const char to copy (if applicable)
 * argSzie   : Number of arguments
 * filename  : file location
 * line      : linenumber
 * stmtStr   : unparsed version of the line to be used for error message
 ********************************************************/
void rted_FunctionCall( const char* fname,
                        const char* /*stmtStr*/,
                        const char* /*leftVar*/,
                        SourceInfo si,
                        size_t argc,
                        const char** args
                      )
{
  RuntimeSystem& rs = RuntimeSystem::instance();

  rs.checkpoint( si );

    // FIXME 2: The current transformation outsputs num (for, e.g. strncat) as
    //    (expr), (size in str)
    // but size in str is off by one -- it includes the null terminator, but
    // should not.

    if( 0 == strcmp("memcpy", fname)) {
      rs.check_memcpy((void*) args[0], (const void*) args[2], num_arg(args, argc, 4));
    } else if ( 0 == strcmp("memmove", fname)) {
      rs.check_memmove( (void*) args[0], (const void*) args[2], num_arg(args, argc, 4));
    } else if ( 0 == strcmp("strcpy", fname)) {
      handle_string_constant(args, 0);
      handle_string_constant(args, 2);

      rs.check_strcpy((char*) args[0], (const char*) args[2]);
    } else if ( 0 == strcmp("strncpy", fname)) {
      rs.check_strncpy((char*) args[0], (const char*) args[2], num_arg(args, argc, 4));
    } else if ( 0 == strcmp("strcat", fname)) {
      handle_string_constant(args, 0);
      handle_string_constant(args, 2);

      rs.check_strcat((char*) args[0], (const char*) args[2]);
    } else if ( 0 == strcmp("strncat", fname)) {
      rs.check_strncat( (char*) args[0], (const char*) args[2], num_arg(args, argc, 4));
    } else if ( 0 == strcmp("strchr", fname)) {
      rs.check_strchr((const char*) args[0], num_arg(args, argc, 2));
    } else if ( 0 == strcmp("strpbrk", fname)) {
      rs.check_strpbrk((const char*) args[0], (const char*) args[2]);
    } else if ( 0 == strcmp("strspn", fname)) {
      rs.check_strspn( (const char*) args[0], (const char*) args[2]);
    } else if ( 0 == strcmp("strstr", fname)) {
      rs.check_strstr((const char*) args[0], (const char*) args[2]);
    } else if ( 0 == strcmp("strlen", fname)) {
      rs.check_strlen((const char*) args[0]);
    } else {
      std::cerr << "Function " << fname << " not yet handled." << std::endl;
      rted_exit(1);
    }
}





/*********************************************************
 * This function is called when one of the following functions in the code is called:
 * fopen, fgetc
 * fname     : function name that is being called
 * args      : arguments to that function
 *           : format : arg1 as string, arg1 as var name (if applicable),
 *           :          arg2 as string, arg2 as var name (if applicable),
 *           :          number of const char to copy (if applicable)
 * argSzie   : Number of arguments
 * filename  : file location
 * line      : linenumber
 * stmtStr   : unparsed version of the line to be used for error message
 ********************************************************/
void rted_IOFunctionCall( const char* fname,
                          const char* /* stmtStr */,
                          const char* /* leftVar */,
                          void* file,
                          const char* arg1,
                          const char* arg2,
                          SourceInfo si
                        )
{
  //fixme - we need to create a new function call that
  // will have FILE* as parameter
  RuntimeSystem& rs = RuntimeSystem::instance();

  checkpoint( rs, si, primaryLoc );

    // not handled (yet?)
    //  fclearerr
    //  feof
    //  ferror
    //  fgetpos
    //  fpos
    //  freopen
    //  fseek
    //  fsetpos
    //  ftell
    //  getchar
    //  putchar
    //  remove
    //  rename
    //  rewind
    //  scanf
    //  setbuf
    //  setvbuf
    //  sprintf
    //  sscanf
    //  tmpfile
    //  tmpnam
    //  ungetc
    //  vfprintf
    //  vprintf
    //  vsprintf

    if ( 0 == strcmp("fclose", fname)) {
      rs.registerFileClose( (FILE*)file );
    } else if ( 0 == strcmp("fflush", fname)) {
      rs.checkFileAccess( (FILE*)file, false /* is_read? */);
    } else if ( 0 == strcmp("fgetc", fname)) {
      rs.checkFileAccess( (FILE*)file, true /* is_read? */);
    } else if ( 0 == strcmp("fgets", fname)) {
      rs.checkFileAccess( (FILE*)file, true /* is_read? */);
    } else if ( 0 == strcmp("fopen", fname)) {
      const char *filen = arg1;
      const char *mode = arg2;
      int openMode=-1;
      if (strcmp(mode,"r")==0)
        openMode=READ;
      if (strcmp(mode,"w")==0)
        openMode=WRITE;
      rs.registerFileOpen((FILE*)file, filen, openMode);
    } else if ( 0 == strcmp("fprintf", fname)) {
      rs.checkFileAccess( (FILE*)file, false /* is_read? */);
    } else if ( 0 == strcmp("fputc", fname)) {
      rs.checkFileAccess( (FILE*)file, false /* is_read? */);
    } else if ( 0 == strcmp("fputs", fname)) {
      rs.checkFileAccess( (FILE*)file, false /* is_read? */);
    } else if ( 0 == strcmp("fread", fname)) {
      rs.checkFileAccess( (FILE*)file, true /* is_read? */);
    } else if ( 0 == strcmp("fscanf", fname)) {
      rs.checkFileAccess( (FILE*)file, true /* is_read? */);
    } else if ( 0 == strcmp("fwrite", fname)) {
      rs.checkFileAccess( (FILE*)file, false /* is_read? */);
    } else if ( 0 == strcmp("getc", fname)) {
      rs.checkFileAccess( (FILE*)file, true /* is_read? */);
    } else if ( 0 == strcmp("putc", fname)) {
      rs.checkFileAccess( (FILE*)file, false /* is_read? */);
    } else if ( 0 == strcmp("::std::fstream", fname)) {
       rs.checkFileAccess((std::fstream&) file, strcmp(arg1,"r") == 0 /* is_read? */);
    } else {
      // \pp does a fall through indicate an error?
    }
}


// ***************************************** FUNCTION CALL *************************************



// ***************************************** SCOPE HANDLING *************************************

void rted_EnterScope(const char* name)
{
  rted_ProcessMsg();
  stats_incr(ctrEnterScope);

  RuntimeSystem& rs = RuntimeSystem::instance();
  rs.getStackManager().beginScope( name );
}

// void rted_ExitScope(size_t scopecount, int delayed, rted_SourceInfo si)
void rted_ExitScope(size_t scopecount, rted_SourceInfo si)
{
  rted_ProcessMsg();
  stats_incr(ctrExitScope);

  RuntimeSystem& rs = RuntimeSystem::instance();

  checkpoint( rs, si, primaryLoc );
  rs.getStackManager().endScope(scopecount);
}

//
// transient pointer handling

void rted_CxxTransientPtr(Address points_to, rted_SourceInfo si)
{
  rted_ProcessMsg();

  RuntimeSystem&   rs = RuntimeSystem::instance();
  checkpoint( rs, si, primaryLoc );

  // get dummy type
  // \todo extend the interface to handle more types
  const RsType* t = rs.getTypeSystem().getTypeInfo("SgTypeInt");
  assert(t);

  // stores the pointer
  rs.getPointerManager().createTransientPointer(points_to, *t);

  // Initially, I planned to split the function exit into a (1) invalidation
  // and a (2) deallocation step. (1) would invalidate all pointers pointing
  // to memory before it goes out of scope. (2) would only remove the memory
  // from the shadow memory. However, this is insufficient as in C++
  // we do not know what memory user defined destructor frees (e.g., heap memory
  // to which an object member points).
  // Thus we wrap functions returning a pointer into a function validating
  // that the memory has not been deallocated.
}

void rted_CTransientPtr(size_t scopecount, Address points_to, rted_SourceInfo si)
{
  rted_CxxTransientPtr(points_to, si);

  // in C++ exitScope is automatically called by the ScopeGuard class
  rted_ExitScope(scopecount, si);
}

void rted_CheckTransientPtr(void** p)
{
  PointerManager& ptrmgr = RuntimeSystem::instance().getPointerManager();

  // the transient pointer is stored internally
  //   thus we only check if it was modified
  if (!ptrmgr.checkTransientPtr())
  {
    // if the object pointed to by the transient pointer was deallocated
    // we set the outside visible pointer to NULL
    *p = 0;
  }

  ptrmgr.clearTransientPtr();
}

void rted_CheckForMemoryLeak(rted_Address location)
{
  PointerManager& ptrmgr = RuntimeSystem::instance().getPointerManager();

  // assuming at least on byte size
  ptrmgr.checkForMemoryLeaks(location, 1);
}


//
// free form error message

void rted_ReportViolation(const char* msg, rted_SourceInfo si)
{
  RuntimeSystem& rs = RuntimeSystem::instance();

  checkpoint( rs, si, primaryLoc );
  rs.violationHandler(RuntimeViolation::OTHER, msg);
}


//
// internal functions

static
int rted_initializeVariable(RuntimeSystem& rs, Address address, size_t sz, const RsType& t)
{
  return rs.checkMemWrite( address, sz, &t );
}

static
int rted_initializePointer(RuntimeSystem& rs, Address address, Address heaploc, const RsPointerType& t)
{
  rted_initializeVariable(rs, address, t.getByteSize(), t);
  rs.registerPointerChange(address, heaploc, t, false);

  // \todo true, only iff the shared region is affected
  return true;
}



// ***************************************** VARIABLE DECLARATIONS *************************************
/*********************************************************
 * This function tells the runtime system that a variable is created
 * we store the type of the variable and whether it has been intialized
 ********************************************************/
int rted_CreateVariable( rted_TypeDesc   td,
                         rted_Address    address,
                         size_t          size,
                         int             init,
                         rted_AllocKind  ak,
                         const char*     name,
                         const char*     mangled_name,
                         const char*     class_name,
                         rted_SourceInfo si
                       )
{
  // CreateVariable is called for stack and global allocations. UPC shared
  // memory allocations need not be broadcast to other UPC threads, b/c each
  // thread will handle them separately. (The RTED startup code initializing
  // the RTED runtime system runs in any UPC thread.)
  rted_ProcessMsg();

  stats_incr(ctrCreateVar);

  RuntimeSystem& rs = RuntimeSystem::instance();
  checkpoint( rs, si, primaryLoc );

  std::string type_name(td.name);
  assert( ("" != type_name) && ("SgArrayType" != type_name) );

  // stack arrays are handled in create array, which is given the dimension
  // information
  const RsType&  rsType =  rs_simpleGetType(rs.getTypeSystem(), type_name, td.base, class_name, td.desc, size);

  // only arrays can be distributed across threads
  // plain variables are solely shared (but not distributed); they reside in thread 0
  const bool     nondistributed = false;
  rs.createVariable(address, name, mangled_name, rsType, ak, nondistributed);

  // done, if not initialized
  if ( ! init ) return 0;

  if (const RsPointerType* pt = dynamic_cast<const RsPointerType*>(&rsType))
  {
    Address heap_address = rted_deref(address, td.desc);

    rted_initializePointer( rs, address, heap_address, *pt);
  }
  else
  {
    // e.g. int x = 3
    // we should flag &x..&x+sizeof(x) as initialized
    rted_initializeVariable( rs, address, size, rsType );
  }

  // can be invoked as part of an expression
  return 0;
}


int rted_CreateObject( TypeDesc td, Address address, size_t sz, SourceInfo si )
{
  rted_ProcessMsg();

  assert(td.desc.shared_mask == 0); // no objects in UPC

  RuntimeSystem&     rs = RuntimeSystem::instance();
  checkpoint( rs, si, primaryLoc );

  const RsType&      rs_type = rs_simpleGetType(rs.getTypeSystem(), td.name, td.base, "", td.desc, sz);
  const RsClassType* rs_classtype = static_cast< const RsClassType* >(&rs_type);

  assert(rs_classtype && rs_classtype->getByteSize() == sz);
  rs.createObject( address, rs_classtype );

  // can be invoked as part of an expression
  return 0;
}


/*********************************************************
 * For a given variable name, check if it is present
 * in the pool of variables created and return mangled_name
 ********************************************************/

int rted_InitVariable( rted_TypeDesc   td,
                       rted_Address    address,
                       size_t          size,
                       int             pointer_changed,
                       const char*     class_name,
                       rted_SourceInfo si
                     )
{
  rted_ProcessMsg();
  stats_incr(ctrInitVar);

  assert(!pointer_changed || strcmp( "SgPointerType", td.name) == 0);

  const Address heap_address = pointer_changed ? rted_deref(address, td.desc) : nullAddr();
  const bool    sendupd = _rted_InitVariable( td, address, heap_address, size, pointer_changed, class_name, si, primaryLoc );

  if (sendupd)
  {
    snd_InitVariable(td, address, heap_address, size, pointer_changed, class_name, si);
  }

  /* can be invoked from expression context */
  return 0;
}


int _rted_InitVariable( rted_TypeDesc    td,
                        rted_Address     address,
                        rted_Address     heap_address,
                        size_t           size,
                        int              pointer_move,
                        const char*      class_name,
                        rted_SourceInfo  si,
                        int              originloc
                      )
{
  RuntimeSystem& rs = RuntimeSystem::instance();

  checkpoint( rs, si, originloc );

  if ( diagnostics::message(diagnostics::variable) )
  {
    std::stringstream message;

    message << "   Init Var at address:  " << address
            << "   type: " << td.name
            << "   size: " << size
            << "   ptrmv: " << pointer_move;
    rs.printMessage(message.str());
  }

  const RsType& rs_type = rs_simpleGetType(rs.getTypeSystem(), td.name, td.base, class_name, td.desc, size);
  int sendupd = 0;

  if (pointer_move)
  {
    rted_initializePointer(rs, address, heap_address, dynamic_cast<const RsPointerType&>(rs_type));
  }
  else
  {
    rted_initializeVariable(rs, address, size, rs_type);
  }

  // can be invoked as part of an expression
  return sendupd;
}


// we want to catch errors like the following:
//    int x[2] = { 0, 1 };
//    int y;
//    int *p = &x[1];
//    p++;
//
//    int q = *p;
void rted_MovePointer( TypeDesc    td,
                       Address     address,
                       const char* class_name,
                       SourceInfo  si
                     )
{
  rted_ProcessMsg();

  Address        heap_address = rted_deref(address, td.desc);

  snd_MovePointer(td, address, heap_address, class_name, si);
  _rted_MovePointer(td, address, heap_address, class_name, si, primaryLoc);
}


void _rted_MovePointer( rted_TypeDesc    td,
                        rted_Address     address,
                        rted_Address     heap_address,
                        const char*      class_name,
                        rted_SourceInfo  si,
                        int              originloc
                      )
{
  RuntimeSystem&       rs = RuntimeSystem::instance();
  checkpoint( rs, si, originloc );

  const size_t         typesz = 0;
  const RsType&        rs_type = rs_simpleGetType(rs.getTypeSystem(), td.name, td.base, class_name, td.desc, typesz);
  const RsPointerType& rp = dynamic_cast<const RsPointerType&>(rs_type);

  assert(rp.getByteSize() > 0); // typesz was not needed
  rs.registerPointerChange( address, heap_address, rp, true );
}



/*********************************************************
 * This function tells the runtime system that a variable is used
 ********************************************************/
void rted_AccessVariable( rted_Address    read_address,
                          size_t          read_size,
                          rted_Address    write_address,
                          size_t          write_size,
                          int             read_write_mask,
                          rted_SourceInfo si
                        )
{
  rted_ProcessMsg();
  stats_incr(ctrAccessVar);

  RuntimeSystem& rs = RuntimeSystem::instance();
  checkpoint( rs, si, primaryLoc );

  if (read_write_mask & Read)  rs.checkMemRead( read_address, read_size );
  if (read_write_mask & Write) rs.checkMemLoc( write_address, write_size );
}

// ***************************************** VARIABLE DECLARATIONS *************************************


// A simple way for users to manually set checkpoints
void rted_Checkpoint(SourceInfo si)
{
  rted_ProcessMsg();

  RuntimeSystem& rs = RuntimeSystem::instance();
  checkpoint( rs, si, primaryLoc );
}

void rted_RegisterTypeCall( const char* nameC,
                            const char* /* typeC */,
                            int isUnion,
                            size_t sizeC,
                            SourceInfo si,
                            size_t argc
                            ...
                          )
{
  // handle the parameters within this call
  va_list        vl;
  // SourceInfo     si = va_arg(vl, SourceInfo);
  // const char*    nameC = va_arg(vl,const char*);
  // /*const char* typeC = */ va_arg(vl,const char*);
  // const char*    isUnionType = va_arg(vl,const char*);
  RuntimeSystem&     rs = RuntimeSystem::instance();
  TypeSystem&        ts = rs.getTypeSystem();

  va_start(vl, argc);

  checkpoint( rs, si, primaryLoc );

  RsClassType&       classType = ts.getClassType( nameC );

  // a stub was registered, we have to fix up its properties
  classType.setByteSize( sizeC );
  classType.setUnionType( isUnion );

  size_t             argctr = 0;

  while (argctr < argc)
  {
      argctr += 4;

      assert(argctr <= argc);

      const RsType* t = NULL;
      std::string   name(va_arg(vl,const char*));
      TypeDesc      td = va_arg(vl,TypeDesc);
      size_t        offset = va_arg(vl, size_t);
      size_t        size = va_arg(vl, size_t);

      if (td.name == std::string("SgArrayType"))
      {
        // \pp was: t = RuntimeSystem_getRsArrayType(ts, &vl, dimensionality, size, base_type );

        assert(argctr < argc);
        ++argctr;  // adjust the loop counter

        const size_t* dimensionality = va_arg( vl, size_t* );
        assert(*dimensionality > 0);

        const size_t typesz = 0;  // \todo get appropriate size
        t = rs_getArrayType( ts, dimensionality, size, rs_getTypeInfo_fallback(ts, td.base, typesz) );
      }
      else
      {
        t = &rs_simpleGetType(ts, td.name, td.base, "", td.desc, size);
      }

      assert(t != NULL);

      if ( diagnostics::message(diagnostics::type) )
      {
        std::stringstream message;

        message << "   Register class-member:  " << name
                << " offset:" << offset
                << " size: " << size;

        rs.printMessage(message.str());
      }

      classType.addMember(name, t, offset);
  }

  va_end(vl);
}

void _rted_FreeMemory(rted_Address addr, rted_AllocKind freeKind, rted_SourceInfo si, int originloc)
{
  RuntimeSystem& rs = RuntimeSystem::instance();

  checkpoint( rs, si, originloc );
  rs.freeMemory( addr, freeKind );
}

void rted_FreeMemory(rted_Address addr, rted_AllocKind freeKind, rted_SourceInfo si)
{
  rted_ProcessMsg();
  snd_FreeMemory( addr, freeKind, si );

  _rted_FreeMemory( addr, freeKind, si, primaryLoc );
}

void rted_ReallocateMemory( void* ptr, size_t size, SourceInfo si )
{
  rted_ProcessMsg();

  RuntimeSystem& rs = RuntimeSystem::instance();
  checkpoint( rs, si, primaryLoc );

  rs.freeMemory( rted_Addr(ptr), akCHeap );

  const long nondistributed = 0;
  rs.createMemory( rted_Addr(ptr), size, akCHeap, nondistributed, NULL );
}


void rted_CheckIfThisNULL(void* thisExp, SourceInfo si)
{
  rted_ProcessMsg();

  RuntimeSystem& rs = RuntimeSystem::instance();

  checkpoint( rs, si, primaryLoc );
  rs.checkIfThisisNULL(thisExp);
}

static inline
size_t digitCount(size_t num)
{
  size_t m = 10;
  size_t n = 1;

  while (m < num)
  {
    ++n;
    m = m*10;
  }

  return n;
}

/// \brief Converts an integer to const char*
/// \deprecated
const char*
rted_ConvertIntToString(size_t num)
{
  const size_t sz = digitCount(num) + 1;
  char*        text = (char*)malloc(sz);

  assert(text);
  snprintf(text, sz, "%lu", ((unsigned long)num));

  return text;
}


#if SCOPE_HANDLING_EXAMPLES

int* foo(int* res)
{
  return res;
}

// Scope Handling
int* major(int id)
{
  switch (id)
  {
    case 0:
      return malloc(sizeof(int));

    case 1:
      return new int();

    case 3:
      return &id;

    case 4:
      return foo(&id);

    default: ;
  }

  return NULL;
}

// transformed code
int* major_c_lang(int id)
{
  enterScope();
  switch (id)
  {
    case 0:
      {
        int* res = malloc(sizeof(int));

        return returnCPtr(1, res);
      }

    case 1:
      {
        int* res = new int();

        return returnCPtr(1, res);
      }

    case 3:
      {
        int* res = &id;

        return returnCPtr(1, res);
      }

    case 4:
      {
        int* res = foo(&id);

        return returnCPtr(1, res);
      }

    default: ;
  }

  return returnCPtr(1, NULL);
  exitScope();
}



#endif
