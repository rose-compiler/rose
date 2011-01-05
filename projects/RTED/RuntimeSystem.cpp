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
#include <numeric>

#include "RuntimeSystem.h"

#include "CppRuntimeSystem/CppRuntimeSystem.h"

#ifdef ROSE_WITH_ROSEQT
#include "CppRuntimeSystem/DebuggerQt/RtedDebug.h"
#endif

static
MemoryType::AllocKind heapAlloc(int fromMalloc)
{
  if (fromMalloc) return MemoryType::CStyleAlloc;

	return MemoryType::CxxStyleAlloc;
}


/**********************************************************
 *  Convert to string
 *********************************************************/
template<typename T>
std::string ToString(T t)
{
  std::ostringstream myStream; //creates an ostringstream object
  myStream << t << std::flush;
  return myStream.str(); //returns the string form of the stringstream object
}

template<typename T>
std::string HexToString(T t)
{
  std::ostringstream myStream; //creates an ostringstream object
  myStream <<  std::hex <<  t ;
  return myStream.str(); //returns the string form of the stringstream object
}

std::string AsString(Address addr, AddressDesc desc)
{
  std::ostringstream myStream; //creates an ostringstream object

  myStream << std::hex << addr ;

	if (desc.shared_mask == MASK_SHARED)
	{
		myStream << "/* shm = " << std::bitset<64>(desc.shared_mask) << "*/ ";
	}

  return myStream.str(); //returns the string form of the stringstream object
}

enum ReadWriteMask { Read = 1, Write = 2, BoundsCheck = 4 };


/*********************************************************
 * This function is closed when RTED finishes (Destructor)
 ********************************************************/
void
rted_RtedClose(char* from) {

  RuntimeSystem * rs = RuntimeSystem::instance();

  rs->doProgramExitChecks();

  std::string stdfrom(from);
  // The runtime system would have exited if it had found an error
  rs->log( "Failed to discover error in RTED test. Origin : "+stdfrom+"\n" );
  exit( 1 );
}


// ***************************************** ARRAY FUNCTIONS *************************************

struct ArrayTypeComputer
{
	typedef std::pair<RsArrayType*, size_t> Operand;

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




// TODO 3 djh: doxygenify
/*
 * Returns an RsArrayType* whose dimensionality dimensions are given by dimDesc and
 * whose base non-array type is base_type.
 */
static
RsArrayType* rs_getArrayType(TypeSystem& ts, const size_t* dimDesc, size_t size, const std::string& base_type)
{
  assert( dimDesc != NULL && *dimDesc > 0 );

  // collect dimension information
	const size_t*       first = dimDesc + 1;
	const size_t*       last = dimDesc + *dimDesc;  // points to the last element (not one past!)
	                                                //   which is the first element being processed
  size_t              no_elems = std::accumulate(first, last+1, static_cast<size_t>(1), std::multiplies<size_t>());
	RsType*             type = ts.getTypeInfo( base_type );
	size_t              elem_size = (size / no_elems);
	size_t              last_sz = elem_size * (*last);
	RsArrayType*        res = ts.getArrayType( type, last_sz );

	return std::accumulate( std::reverse_iterator<const size_t*>(last),
	                        std::reverse_iterator<const size_t*>(first),
													ArrayTypeComputer::Operand(res, last_sz),
													ArrayTypeComputer(ts)
												).first;
}


static
RsType* rs_getType( TypeSystem& ts,
                    std::string type,
				            std::string base_type,
				            std::string class_name,
				            AddressDesc desc
									)
{
  if( type == "SgClassType" )
    type = class_name;
  else if( base_type == "SgClassType" ) {
    base_type = class_name;
    assert( base_type != "" );
  }
  assert( type != "" );

  RsType*        res = NULL;

  if( type == "SgPointerType" ) {
    RsType* bt = NULL;
    assert( desc.levels > 0 );

    bt = ts.getTypeInfo(base_type);

    if( bt == NULL ) {
      // Create a type stub.  This will result in errors later if it is not
      // fixed up by a subsequent registration (i.e., of that type).
      bt = new RsClassType( base_type, 0, false );
      ts.registerType( bt );
    }

    assert( bt != NULL );

    res = ts.getPointerType(bt, desc);
  } else {
    res = ts.getTypeInfo( type );

    if (res == NULL) {
      // Create a type stub.  This will result in errors later if it is not
      // fixed up by a subsequent registration (i.e., of that type).
      res = new RsClassType( type, 0, false );
      ts.registerType( res );
    }
  }

  assert( res );
  return res;
}

static bool initialize_next_array = false;

// FIXME 3: This is not threadsafe.  At the moment, because createVariable and
// createArray are called for stack and pointer arrays and because
// createVariable can't call rs -> createArray without the dimension
// information, this hack exists.  It will not be necessary after the
// transformation is refactored.

/*********************************************************
 * This function is called when an array is created
 *   (the shared memory version was copied form rted_CreateHeap)
 * name      : variable name
 * manglname : variable mangl_name
 * type      : Sage Type
 * dimension : 1 or 2
 * sizeA     : size of dimension 1
 * sizeB     : size of dimension 2
 * ismalloc  : Allocated through malloc?
 * filename  : file location
 * line      : linenumber
 ********************************************************/

void rted_CreateHeapArr( TypeDesc      td,
												 Address       address,
												 size_t        elemsize,
												 size_t        totalsize,
												 const size_t* dimDescr,
												 const char*   name,
												 const char*   mangl_name,
												 const char*   class_name,
												 SourceInfo    si
											 )
{
	assert(std::string("SgArrayType") == td.name);

  RuntimeSystem * rs = RuntimeSystem::instance();
  rs->checkpoint(SourcePosition(si));

  std::string     base_type(td.base);
  if( base_type == "SgClassType" )
    base_type = class_name;

	// Aug 6 : TODO : move this to createVariable
	RsArrayType* rstype = rs_getArrayType(*rs->getTypeSystem(), dimDescr, totalsize, base_type);

	rs -> createArray( address, name, mangl_name, rstype );

	if (initialize_next_array) {
		rs -> checkMemWrite( address, elemsize );
		initialize_next_array = false;
	}
}

void rted_CreateHeapPtr( TypeDesc    td,
												 Address     address,
												 size_t      /*elemsize*/,
												 size_t      mallocSize,
												 int         fromMalloc,
												 const char* class_name,
												 SourceInfo  si
											 )
{
	assert(std::string("SgPointerType") == td.name);

  RuntimeSystem * rs = RuntimeSystem::instance();
  rs->checkpoint(SourcePosition(si));

  std::string base_type(td.base);
  if( base_type == "SgClassType" )
    base_type = class_name;

	Address        heap_address = ptr_deref(address, td.desc);
	RsType*        rs_type = rs_getType(*rs->getTypeSystem(), td.name, td.base, class_name, td.desc);
	RsPointerType* rs_ptr_type = static_cast< RsPointerType* >(rs_type);
	RsClassType*   class_type = dynamic_cast< RsClassType* >(rs_ptr_type->getBaseType());


	std::cerr << " registering heap type:" << td.name
						<< "   basetype: " << td.base
						<< "   class_name: " << class_name
						<< "   indirection_level: " << ToString(td.desc.levels)
						<< "   address: " << AsString(heap_address, pd_deref(td.desc))
						<< "   malloc size: " << ToString(mallocSize)
						<< std::endl;

	// A class might have had its memory allocation registered in the
	// constructor.  If there was no explicit constructor, however, we still
	// have to allocate the memory here.
	if(!class_type || rs->getMemManager()->findContainingMem(heap_address) == NULL)
	{
		// FIXME 2: This won't handle the unlikely case of a C++ object being
		// allocated via malloc and then freed with delete.

		rs -> createMemory( heap_address, mallocSize, heapAlloc(fromMalloc) );
	}

	rs -> registerPointerChange(
			address,
			heap_address,
			rs_ptr_type,
			false,  // checkPtrMove? no, pointer may change regions
			true    // checkMemLeak? yes
			);
}


static
void rs_checkMemoryAccess(RuntimeSystem& rs, Address address, size_t size, int read_write_mask)
{
  if (read_write_mask & Read)
    rs.checkMemRead( address, size );

  if (read_write_mask & Write)
    rs.checkMemWrite( address, size );
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
void rted_AccessHeap( Address base_address, // &( array[ 0 ])
										  Address address,
											size_t size,
											int read_write_mask,  // 1 = read, 2 = write
											SourceInfo si
										)
{
  RuntimeSystem * rs = RuntimeSystem::instance();
  rs->checkpoint(SourcePosition(si));

	rs_checkMemoryAccess(*rs, address, size, read_write_mask);

  if( read_write_mask & BoundsCheck ) {
    rs->getMemManager()->checkIfSameChunk(base_address, address, size);
  }
}

// ***************************************** FUNCTION CALL *************************************

struct AsRsType
{
	TypeSystem& ts;

	explicit
	AsRsType(TypeSystem& typesys)
	: ts(typesys)
	{}

	RsType* operator()(const TypeDesc& td)
	{
		return rs_getType(ts, td.name, td.base, "", td.desc);
	}
};

void rted_AssertFunctionSignature( const char* name,
																	 size_t type_count,
																	 TypeDesc* typedescs,
                                   SourceInfo si
																 )
{
  RuntimeSystem * rs = RuntimeSystem::instance();

  rs -> checkpoint(SourcePosition(si));

  std::vector< RsType* > types;

  types.reserve(type_count);
	std::transform(typedescs, typedescs + type_count, std::back_inserter(types), AsRsType(*rs->getTypeSystem()));
  rs->expectFunctionSignature(name, types);
}

void rted_ConfirmFunctionSignature(const char* name, size_t type_count, TypeDesc* typedescs)
{
  RuntimeSystem * rs = RuntimeSystem::instance();

  std::vector< RsType* > types;

  types.reserve(type_count);
  std::transform(typedescs, typedescs + type_count, std::back_inserter(types), AsRsType(*rs->getTypeSystem()));
  rs->confirmFunctionSignature( name, types );
}


static
void RuntimeSystem_ensure_allocated_and_initialized( Address addr, size_t size)
{
  RuntimeSystem * rs = RuntimeSystem::instance();

  // We trust that anything allocated is properly initialized -- we're not
  // working around a string constant so there's no need for us to do anything.
  if(rs->getMemManager()->findContainingMem(addr) != NULL)
    return;

  // \pp ???
  rs->createArray(addr, "StringConstant", "MangledStringConstant", "SgTypeChar", size);
  rs->checkMemWrite(addr, size);
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
		Address addr;

		addr.local = (char*) args[i];

    RuntimeSystem_ensure_allocated_and_initialized(addr, numval(args[ i + 1 ]));
	}
}

static inline
int num_arg(const char** args, size_t noArgs, size_t i)
{
	assert (noArgs > 0);

	// \pp ???
	if (i < noArgs - 1 && isdigit( args[ i + 1 ][ 0 ])) ++i;

  return numval(args[i]);
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
static
void RuntimeSystem_handleSpecialFunctionCalls( const char* fname,
																							 const char** args,
																							 int argsSize,
																							 SourceInfo si
																						 )
{
  RuntimeSystem * rs = RuntimeSystem::instance();

  rs -> checkpoint( SourcePosition( si));

    // FIXME 2: The current transformation outsputs num (for, e.g. strncat) as
    //    (expr), (size in str)
    // but size in str is off by one -- it includes the null terminator, but
    // should not.

    if( 0 == strcmp("memcpy", fname)) {
      rs->check_memcpy((void*) args[0], (const void*) args[2], num_arg(args, argsSize, 4));
    } else if ( 0 == strcmp("memmove", fname)) {
      rs->check_memmove( (void*) args[0], (const void*) args[2], num_arg(args, argsSize, 4));
    } else if ( 0 == strcmp("strcpy", fname)) {
      handle_string_constant(args, 0);
      handle_string_constant(args, 2);

      rs->check_strcpy((char*) args[0], (const char*) args[2]);
    } else if ( 0 == strcmp("strncpy", fname)) {
      rs->check_strncpy((char*) args[0], (const char*) args[2], num_arg(args, argsSize, 4));
    } else if ( 0 == strcmp("strcat", fname)) {
      handle_string_constant(args, 0);
      handle_string_constant(args, 2);

      rs->check_strcat((char*) args[0], (const char*) args[2]);
    } else if ( 0 == strcmp("strncat", fname)) {
      rs->check_strncat( (char*) args[0], (const char*) args[2], num_arg(args, argsSize, 4));
    } else if ( 0 == strcmp("strchr", fname)) {
      rs->check_strchr((const char*) args[0], num_arg(args, argsSize, 2));
    } else if ( 0 == strcmp("strpbrk", fname)) {
      rs->check_strpbrk((const char*) args[0], (const char*) args[2]);
    } else if ( 0 == strcmp("strspn", fname)) {
      rs->check_strspn( (const char*) args[0], (const char*) args[2]);
    } else if ( 0 == strcmp("strstr", fname)) {
      rs->check_strstr((const char*) args[0], (const char*) args[2]);
    } else if ( 0 == strcmp("strlen", fname)) {
      rs->check_strlen((const char*) args[0]);
    } else {
      std::cerr << "Function " << fname << " not yet handled." << std::endl;
      exit(1);
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
  RuntimeSystem * rs = RuntimeSystem::instance();

  rs -> checkpoint( SourcePosition(si));

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
      rs -> registerFileClose( (FILE*)file );
    } else if ( 0 == strcmp("fflush", fname)) {
      rs -> checkFileAccess( (FILE*)file, false /* is_read? */);
    } else if ( 0 == strcmp("fgetc", fname)) {
      rs -> checkFileAccess( (FILE*)file, true /* is_read? */);
    } else if ( 0 == strcmp("fgets", fname)) {
      rs -> checkFileAccess( (FILE*)file, true /* is_read? */);
    } else if ( 0 == strcmp("fopen", fname)) {
      const char *filen = arg1;
      const char *mode = arg2;
      int openMode=-1;
      if (strcmp(mode,"r")==0)
        openMode=READ;
      if (strcmp(mode,"w")==0)
        openMode=WRITE;
      rs->registerFileOpen((FILE*)file, filen, openMode);
    } else if ( 0 == strcmp("fprintf", fname)) {
      rs -> checkFileAccess( (FILE*)file, false /* is_read? */);
    } else if ( 0 == strcmp("fputc", fname)) {
      rs -> checkFileAccess( (FILE*)file, false /* is_read? */);
    } else if ( 0 == strcmp("fputs", fname)) {
      rs -> checkFileAccess( (FILE*)file, false /* is_read? */);
    } else if ( 0 == strcmp("fread", fname)) {
      rs -> checkFileAccess( (FILE*)file, true /* is_read? */);
    } else if ( 0 == strcmp("fscanf", fname)) {
      rs -> checkFileAccess( (FILE*)file, true /* is_read? */);
    } else if ( 0 == strcmp("fwrite", fname)) {
      rs -> checkFileAccess( (FILE*)file, false /* is_read? */);
    } else if ( 0 == strcmp("getc", fname)) {
      rs -> checkFileAccess( (FILE*)file, true /* is_read? */);
    } else if ( 0 == strcmp("putc", fname)) {
      rs -> checkFileAccess( (FILE*)file, false /* is_read? */);
    }
     else if ( 0 == strcmp("std::fstream", fname)) {
       if (0 == strcmp(arg1,"r"))
				 rs -> checkFileAccess((std::fstream&) file,true /* is_read? */);
       else
				 rs -> checkFileAccess((std::fstream&) file,false /* is_read? */);
    }
}





/*********************************************************
 * This function is called when a function is called
 * The number of parameters is variable but some of the are fixed:
 * count        : number of parameters
 * name         : variable name if it is a variable to be put on the stack
 *              : function name if it is a function that is being called
 * mangled_name : mangled name of the above
 * scope        : Scope indicator (might not be necessary)
 * insertBefore : Indicates whether to push or pop a variable form the stack
 * filename     : file location
 * line         : linenumber
 * stmtStr      : unparsed version of the line to be used for error message
 * ...          : variable amount of additional parameters
 ********************************************************/
void
rted_FunctionCall(size_t count, ...) {
  // handle the parameters within this call
  va_list vl;
  va_start(vl, count);

	// \pp was: const char** args = (const char**)malloc(sizeof(const char*)*count+1);
	const char* args[count+1];

  size_t      posArgs = 0;
  const char* name = NULL;
	SourceInfo  si;
  const char* stmtStr = NULL;
  const char* leftVar = NULL;

	si.file = NULL; si.src_line = 0; si.rted_line = 0;

  for (size_t i=0; i < count; ++i) {
		switch (i)
		{
			case 0: name = va_arg(vl, const char*); break;
			case 1: si = va_arg(vl, SourceInfo); break;
			case 2: stmtStr = va_arg(vl, const char*); break;
			case 3: leftVar = va_arg(vl, const char*); break;
			default:
			  args[posArgs] = va_arg(vl, const char*);
				++posArgs;
	  }
  }
  va_end(vl);


  //if (RuntimeSystem_isInterestingFunctionCall(name)==1) {
  // if the string name is one of the above, we handle it specially
  RuntimeSystem_handleSpecialFunctionCalls(name, args, posArgs, si /*, stmtStr, leftVar*/);


	// \pp probably free(args) is missing

  // } else if (RuntimeSystem_isFileIOFunctionCall(name)==1) {
  // this will be replaced by a direct call
  //RuntimeSystem_handleIOFunctionCall(name, args, posArgs, filename, line, lineTransf, stmtStr, leftVar, NULL);
  //} else {
  //printMessage("Unknown Function call to RuntimeSystem!\n");
  //  exit(1);
  //}
}



// ***************************************** FUNCTION CALL *************************************



// ***************************************** SCOPE HANDLING *************************************

void rted_EnterScope(const char* name) {

  RuntimeSystem * rs = RuntimeSystem::instance();
  rs -> beginScope( name );

}

void rted_ExitScope(const char*, SourceInfo si)
{
  RuntimeSystem * rs = RuntimeSystem::instance();

	rs -> checkpoint( SourcePosition( si ));
	rs -> endScope();
}




// ***************************************** SCOPE HANDLING *************************************


// ***************************************** VARIABLE DECLARATIONS *************************************
/*********************************************************
 * This function tells the runtime system that a variable is created
 * we store the type of the variable and whether it has been intialized
 ********************************************************/
int rted_CreateVariable( const char* name,
												 const char* mangled_name,
												 TypeDesc td,
												 Address address,
												 size_t size,
												 int init,
												 const char* class_name,
												 SourceInfo si
						           )
{
  RuntimeSystem * rs = RuntimeSystem::instance();
  rs -> checkpoint( SourcePosition( si ));

	std::string type_name(td.name);
  assert( type_name != "" );

  // stack arrays are handled in create array, which is given the dimension
  // information
  if (type_name != "SgArrayType") {
    RsType * rsType = rs_getType(*rs->getTypeSystem(), type_name, td.base, class_name, td.desc);

    // tps : (09/04/2009) : It seems to be allowed for the type to be NULL
    //  in order to register new types
    // if (rsType==NULL) cerr <<" type: " << type_name << " unknown " << endl;

		assert(rsType);
    rs->createVariable(address,name,mangled_name,rsType);
  }


  if ( 1 == init ) {
    // e.g. int x = 3
    // we should flag &x..&x+sizeof(x) as initialized

    if( type_name == "SgArrayType" )
      initialize_next_array = true;
    else
      rs -> checkMemWrite( address, size );
  }

  return 0;
}


void rted_CreateObject( TypeDesc td, Address address, SourceInfo si )
{
	assert(td.desc.shared_mask == 0); // no objects in UPC

  RuntimeSystem * rs = RuntimeSystem::instance();
  rs -> checkpoint( SourcePosition( si ));

	RsType*         rs_type = rs_getType(*rs->getTypeSystem(), td.name, td.base, "", td.desc);
  RsClassType*    rs_classtype = static_cast< RsClassType* >(rs_type);

	assert(rs_classtype);
  rs->createObject( address, rs_classtype );
}


/*********************************************************
 * For a given variable name, check if it is present
 * in the pool of variables created and return mangled_name
 ********************************************************/
void rted_InitVariable( TypeDesc td,
			                  Address address,
			                  size_t size,
												const char* class_name,
			                  int ismalloc,
			                  int pointer_changed,
			                  SourceInfo si
											)
{
  RuntimeSystem * rs = RuntimeSystem::instance();
  rs -> checkpoint( SourcePosition( si ));

  std::string message = "   Init Var at address:  "
	                      + HexToString(address)
												+ "  type:"
                        + td.name
												+ "   size: "
												+ ToString(size);
  rs->printMessage(message);

  RsType* rs_type = rs_getType(*rs->getTypeSystem(), td.name, td.base, class_name, td.desc);
  rs -> checkMemWrite( address, size, rs_type );

  // This assumes roseInitVariable is called after the assignment has taken
  // place (otherwise we wouldn't get the new heap address).
  //
  // Note that we cannot call registerPointerChange until after the memory
  // creation is registered, which is done in roseCreateHeap.
  //cerr << "============ Possible pointer change : ismalloc: "<<ismalloc<<
  // "   pointer_changed:" << pointer_changed << " pointer_type:" <<
  //  type<<endl;

  if(  ismalloc != 1
	  && pointer_changed == 1
	  && 0 == strcmp( "SgPointerType", td.name)
		)
	{
		// \pp \todo the following deref needs to know about the shared mask
    Address heap_address = ptr_deref(address, pd_ptr());
    rs -> registerPointerChange( address, heap_address, rs_type, false, true );
  }
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
  RuntimeSystem * rs = RuntimeSystem::instance();
  rs -> checkpoint( SourcePosition( si ));

  Address heap_address = ptr_deref(address, td.desc);
  RsType* rs_type = rs_getType(*rs->getTypeSystem(), td.name, td.base, class_name, td.desc);

  rs -> registerPointerChange( address, heap_address, rs_type, true, false );
}


/*********************************************************
 * This function tells the runtime system that a variable is used
 ********************************************************/
void rted_AccessVariable( Address address,
													size_t size,
													Address write_address,
													size_t write_size,
													int read_write_mask,
													SourceInfo si
				                )
{
  RuntimeSystem * rs = RuntimeSystem::instance();
  rs->checkpoint( SourcePosition( si ));

	rs_checkMemoryAccess(*rs, address, size, read_write_mask & Read);
  rs_checkMemoryAccess(*rs, write_address, write_size, read_write_mask & Write);
}

// ***************************************** VARIABLE DECLARATIONS *************************************


// A simple way for users to manually set checkpoints
void rted_Checkpoint(SourceInfo si) {
  RuntimeSystem * rs = RuntimeSystem::instance();
  rs -> checkpoint( SourcePosition( si ));
}


void
rted_RegisterTypeCall(size_t count, ...) {
  // handle the parameters within this call
  va_list        vl;

	va_start(vl, count);

  SourceInfo     si = va_arg(vl, SourceInfo);
  const char*    nameC = va_arg(vl,const char*);
  /*const char* typeC = */ va_arg(vl,const char*);
  const char*    isUnionType = va_arg(vl,const char*);
  bool           isUnion = false;

	if (*isUnionType=='1') isUnion=true;

	// \pp was: addr_type sizeC = va_arg(vl, addr_type);
	size_t         sizeC = numval(va_arg(vl, char*));
  RuntimeSystem* rs = RuntimeSystem::instance();
	TypeSystem&    ts = *rs->getTypeSystem();

  rs -> checkpoint( SourcePosition( si ));

  RsClassType*   classType = static_cast< RsClassType* >(ts.getTypeInfo( nameC ));

  if( NULL == classType )  {
    // no stub has been registered
    classType = new RsClassType(nameC, sizeC, isUnion);
    ts.registerType(classType);
  } else {
    // a stub was registered, we have to fix up its properties
    classType -> setByteSize( sizeC );
    classType -> setUnionType( isUnion );
  }

	size_t          argctr = 7;

	while (argctr < count)
	{
		  RsType*     t = NULL;
      std::string name(va_arg(vl,const char*));
      std::string type(va_arg(vl,const char*));
      std::string base_type(va_arg(vl,const char*));
      AddressDesc addrDesc = va_arg( vl, AddressDesc );
      size_t      offset = va_arg(vl, size_t);
      size_t      size = va_arg(vl, size_t);

			argctr += 6;

			if (type == "SgArrayType")
			{
				// \pp was: t = RuntimeSystem_getRsArrayType(ts, &vl, dimensionality, size, base_type );

				const size_t dimensionality = va_arg( vl, size_t );
				size_t       dims[dimensionality+1];
				size_t       pos = 0;

        assert(dimensionality > 0);
				argctr += dimensionality + 1;  // adjust the loop counter
				dims[pos] = dimensionality;

				while (++pos < dimensionality+1)
				{
					dims[pos] = va_arg( vl, size_t );
			  }

				t = rs_getArrayType(ts, dims, size, base_type);
      }
			else
			{
        t = rs_getType(ts, type, base_type, "", addrDesc);
      }

      assert(t != NULL);
      std::string message = "   Register class-member:  " + name + "  offset:"
                            + HexToString(offset)+ "   size: " + ToString(size);

			rs->printMessage(message);
      classType->addMember(name, t, offset);

      //cerr << "Registering Member " << name << " of type " << type << " at offset " << offset << endl;
  }

	assert(argctr == count);
	va_end(vl);
}

void
rted_FreeMemory( void* ptr,              ///< the address that is about to be freed
                 int fromMalloc,         ///< whether the free expects to be paired with
                                         ///  memory allocated via 'malloc'.  In short,
                                         ///  whether this is a call to free (1) or delete
                                         ///  (0)
                 SourceInfo si
							 )
{
  RuntimeSystem * rs = RuntimeSystem::instance();

	rs->checkpoint( SourcePosition(si));
	rs->freeMemory( memAddr(ptr), heapAlloc(fromMalloc) );
}


void rted_ReallocateMemory( void* ptr, size_t size, SourceInfo si )
{
  RuntimeSystem * rs = RuntimeSystem::instance();
  rs->checkpoint( SourcePosition(si));

	rs->freeMemory( memAddr(ptr), MemoryType::CStyleAlloc );
	rs->createMemory( memAddr(ptr), size, MemoryType::CStyleAlloc);
}



void rted_CheckIfThisNULL( void* thisExp, SourceInfo si)
{
	RuntimeSystem * rs = RuntimeSystem::instance();

	rs->checkpoint( SourcePosition( si ));
	rs->checkIfThisisNULL(thisExp);
}


#if OBSOLETE_CODE

extern int RuntimeSystem_original_main(int argc, char**argv, char**envp);


int main(int argc, char **argv, char ** envp) {

  int exit_code = RuntimeSystem_original_main(argc, argv, envp);
  rted_RtedClose((char*)"RuntimeSystem.cpp:main");

  return exit_code;
}


void rted_CreateHeapArr( TypeDesc      td,
												 Address       address,
												 size_t        elemsize,
												 size_t        totalsize,
												 const size_t* dimDescr,
												 const char*   name,
												 const char*   mangl_name,
												 const char*   class_name,
												 SourceInfo    si
											 );


/*********************************************************
 * This function is called when an array is created
 * name      : variable name
 * manglname : variable mangl_name
 * type      : Sage Type
 * dimension : 1 or 2
 * sizeA     : size of dimension 1
 * sizeB     : size of dimension 2
 * ismalloc  : Allocated through malloc?
 * filename  : file location
 * line      : linenumber
 ********************************************************/
void
rted_CreateHeap( const char* name,
                 const char* mangl_name,
								 const char* type,
								 const char* basetype,
								 size_t indirection_level,
								 addr_type address,
								 long int size,
								 long int mallocSize,
								 int fromMalloc,
								 const char* class_name,
								 const char* filename,
								 size_t line,
								 size_t lineTransformed,
								 const size_t* dimDesc
							 )
						{


  RuntimeSystem * rs = RuntimeSystem::instance();
  rs -> checkpoint( SourcePosition( filename, line, lineTransformed));

    string type_name = type;
  string base_type = basetype;
  if( base_type == "SgClassType" )
    base_type = class_name;


  if( type_name == "SgArrayType" ) {
    // Aug 6 : TODO : move this to createVariable
    RsArrayType* type = RuntimeSystem_getRsArrayType( dimDesc, size, base_type );

    rs -> createArray( address, name, mangl_name, type );

    if( initialize_next_array ) {
      rs -> checkMemWrite( address, size );
      initialize_next_array = false;
    }
  } else if( type_name == "SgPointerType") {
    addr_type heap_address = *((addr_type*) address);
    cerr << " registering heap   type:" << type << "  basetype:"<<basetype<<
    		"  class_name:" <<class_name<<"  indirection_level:"<<ToString(indirection_level)<<
    		"  address:"<<HexToString(heap_address) <<"  malloc size:"<<ToString(mallocSize)<<endl;

    RsPointerType* rs_type
      = static_cast< RsPointerType* >(
          RuntimeSystem_getRsType(
							type,
							basetype,
							class_name,
							indirection_level ));
    RsClassType* class_type
      = dynamic_cast< RsClassType* >( rs_type -> getBaseType() );
    // A class might have had its memory allocation registered in the
    // constructor.  If there was no explicit constructor, however, we still
    // have to allocate the memory here.
    if(   !class_type
          || rs -> getMemManager() -> findContainingMem( heap_address) == NULL ) {
      // FIXME 2: This won't handle the unlikely case of a C++ object being
      // allocated via malloc and then freed with delete.
      //
      // object memory allocation is handled in the constructor
      bool was_from_malloc = ( fromMalloc == 1 );
      rs -> createMemory( heap_address, mallocSize, false, was_from_malloc );
    }
    rs -> registerPointerChange(
				address,
				heap_address,
        rs_type,
				false,  // checkPtrMove? no, pointer may change regions
				true    // checkMemLeak? yes
				);
  } else {
    cerr << "Unexpected Array Type: " << type << endl;
    exit( 1 );
  }
}

#endif /* OBSOLETE_CODE */

#if UNUSED_CODE

/*********************************************************
 * Check if a function call is interesting, i.e. contains a
 * call to a function that we need to check the parameters of
 ********************************************************/
static
int RuntimeSystem_isInterestingFunctionCall(const char* name) {
  int interesting=0;//false;
  if ( ( strcmp(name,"memcpy")==0 ||
         strcmp(name ,"memmove")==0 ||
	 strcmp(name ,"strcpy")==0 ||
	 strcmp(name ,"strncpy")==0 ||
	 strcmp(name ,"strcat")==0 ||
	 strcmp(name ,"strncat")==0 ||
	 strcmp(name ,"strlen")==0 ||
	 strcmp(name ,"strchr")==0 ||
	 strcmp(name ,"strpbrk")==0 ||
	 strcmp(name ,"strspn")==0  ||
	 strcmp(name ,"strstr")==0
	 )) {
    interesting=1;
  }
  return interesting;
}

/*********************************************************
 * Convert an integer to const char*
 ********************************************************/
const char*
rted_ConvertIntToString(int t) {
  int size = sizeof(int);
  char* text = (char*)malloc(size+1);
  if (text)
    sprintf(text,"%d",t);
  //printMessage("String converted from int : %s ",text);
  return text;
}

#endif /* UNUSED_CODE */
