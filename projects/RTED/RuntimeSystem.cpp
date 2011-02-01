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
#include "ParallelRTS.h"

#include "CppRuntimeSystem/CppRuntimeSystem.h"

#ifdef ROSE_WITH_ROSEQT
#include "CppRuntimeSystem/DebuggerQt/RtedDebug.h"
#endif


/**********************************************************
 *  Convert to string
 *********************************************************/

static
std::string AsString(Address addr, AddressDesc desc)
{
  std::ostringstream myStream; //creates an ostringstream object

	if ((desc.shared_mask & MASK_SHARED) == MASK_SHARED)
	{
		myStream << "/* shm = " << std::bitset<64>(desc.shared_mask) << "*/ ";
	}
	else
	{
		myStream << std::hex << ((void*)addr.local);
  }

  return myStream.str(); //returns the string form of the stringstream object
}

enum ReadWriteMask { Read = 1, Write = 2, BoundsCheck = 4 };


/*********************************************************
 * This function is closed when RTED finishes (Destructor)
 ********************************************************/
void rted_Close(char* from)
{
  RuntimeSystem * rs = RuntimeSystem::instance();

  rs->doProgramExitChecks();

  std::string stdfrom(from);
  // The runtime system would have exited if it had found an error
  rs->log( "Failed to discover error in RTED test. Origin : "+stdfrom+"\n" );
  // exit( 1 );
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

void  rted_CreateHeapArr( rted_TypeDesc      td,
												  rted_Address       address,
												  size_t             elemsize,    /* \pp always 0 ? */
												  size_t             totalsize,
												  const size_t*      dimDescr,
												  const char*        name,
												  const char*        mangl_name,
												  const char*        class_name,
												  rted_SourceInfo    si
											  )
{
	assert(std::string("SgArrayType") == td.name);

  RuntimeSystem * rs = RuntimeSystem::instance();
  rs->checkpoint(SourcePosition(si) );

  std::string     base_type(td.base);
  if( base_type == "SgClassType" )
    base_type = class_name;

	// Aug 6 : TODO : move this to createVariable
	RsArrayType* rstype = rs_getArrayType(*rs->getTypeSystem(), dimDescr, totalsize, base_type);

	rs -> createArray( address, td.desc, name, mangl_name, rstype );

	if (initialize_next_array) {
		rs -> checkMemWrite( address, td.desc, elemsize );
		initialize_next_array = false;
	}
}



void _rted_CreateHeapPtr( rted_TypeDesc    td,
											    rted_Address     address,
											    size_t           size,
											    size_t           mallocSize,
											    rted_AllocKind   allocKind,
											    const char*      class_name,
											    rted_SourceInfo  si
											  )
{
  assert(std::string("SgPointerType") == td.name);

  RuntimeSystem * rs = RuntimeSystem::instance();
  rs->checkpoint(SourcePosition(si) );

  std::string base_type(td.base);
  if( base_type == "SgClassType" )
    base_type = class_name;

	RsType*        rs_type = rs_getType(*rs->getTypeSystem(), td.name, td.base, class_name, td.desc);
	RsPointerType* rs_ptr_type = static_cast< RsPointerType* >(rs_type);
	RsClassType*   class_type = dynamic_cast< RsClassType* >(rs_ptr_type->getBaseType());
	Address        heap_address = rted_deref(address, td.desc);
	AddressDesc    heap_desc = rted_deref_desc(td.desc);

	std::cerr << " registering heap type:" << td.name
						<< "   basetype: " << td.base
						<< "   class_name: " << class_name
						<< "   indirection_level: " << ToString(td.desc.levels)
						<< "   address: " << AsString(heap_address, heap_desc)
						<< "   malloc size: " << ToString(mallocSize)
						<< std::endl;

	// A class might have had its memory allocation registered in the
	// constructor.  If there was no explicit constructor, however, we still
	// have to allocate the memory here.
	MemoryManager::Location loc = rted_system_addr(heap_address, heap_desc);

  std::cerr << " <<<<:" << HexToString(loc) << "  -- " << allocKind << std::endl;

	if(!class_type || rs->getMemManager()->findContainingMem(loc, 1) == NULL)
	{
		// FIXME 2: This won't handle the unlikely case of a C++ object being
		// allocated via malloc and then freed with delete.

		rs -> createMemory( heap_address, mallocSize, allocKind );
	}

	rs -> registerPointerChange(
			address,
			td.desc,
			heap_address,
			heap_desc,
			rs_ptr_type,
			false,  // checkPtrMove? no, pointer may change regions
			true    // checkMemLeak? yes
			);
}

void rted_CreateHeapPtr( rted_TypeDesc    td,
												 rted_Address     address,
												 size_t           size,
												 size_t           mallocSize,
												 rted_AllocKind   allocKind,
												 const char*      class_name,
												 rted_SourceInfo  si
											 )
{
	rted_ProcessMsg();
	snd_CreateHeapPtr(td, address, size, mallocSize, allocKind, class_name, si);
	_rted_CreateHeapPtr(td, address, size, mallocSize, allocKind, class_name, si);
}


static
void rs_checkMemoryAccess(RuntimeSystem& rs, Address addr, AddressDesc desc, size_t size, int read_write_mask)
{
  if (read_write_mask & Read)
    rs.checkMemRead( addr, desc, size );

  if (read_write_mask & Write)
    rs.checkMemWrite( addr, desc, size );
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
void rted_AccessHeap( rted_Address     base_address, // &( array[ 0 ])
										  rted_Address     address,
											size_t           size,
											rted_AddressDesc desc,
											int              read_write_mask,  // 1 = read, 2 = write
											rted_SourceInfo  si
										)
{
	rted_ProcessMsg();

  RuntimeSystem * rs = RuntimeSystem::instance();
  rs->checkpoint(SourcePosition(si) );

	rs_checkMemoryAccess(*rs, address, desc, size, read_write_mask);

  if( read_write_mask & BoundsCheck )
	{
		MemoryManager::Location loc_base = rted_system_addr(base_address, desc);
		MemoryManager::Location loc_addr = rted_system_addr(address, desc);

    rs->getMemManager()->checkIfSameChunk(loc_base, loc_addr, size);
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

  rs -> checkpoint( SourcePosition(si) );

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
  RuntimeSystem *         rs = RuntimeSystem::instance();

  // we assume a local address for C system functions
	const AddressDesc       desc = rted_ptr();

	// We trust that anything allocated is properly initialized -- we're not
  // working around a string constant so there's no need for us to do anything.
	MemoryManager::Location loc = rted_system_addr(addr, desc);
  if(rs->getMemManager()->findContainingMem(loc, 1) != NULL)
    return;

  // \pp ???
  rs->createArray(addr, desc, "StringConstant", "MangledStringConstant", "SgTypeChar", size);
  rs->checkMemWrite(addr, desc, size);
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
  RuntimeSystem * rs = RuntimeSystem::instance();

  rs -> checkpoint( SourcePosition( si));

    // FIXME 2: The current transformation outsputs num (for, e.g. strncat) as
    //    (expr), (size in str)
    // but size in str is off by one -- it includes the null terminator, but
    // should not.

    if( 0 == strcmp("memcpy", fname)) {
      rs->check_memcpy((void*) args[0], (const void*) args[2], num_arg(args, argc, 4));
    } else if ( 0 == strcmp("memmove", fname)) {
      rs->check_memmove( (void*) args[0], (const void*) args[2], num_arg(args, argc, 4));
    } else if ( 0 == strcmp("strcpy", fname)) {
      handle_string_constant(args, 0);
      handle_string_constant(args, 2);

      rs->check_strcpy((char*) args[0], (const char*) args[2]);
    } else if ( 0 == strcmp("strncpy", fname)) {
      rs->check_strncpy((char*) args[0], (const char*) args[2], num_arg(args, argc, 4));
    } else if ( 0 == strcmp("strcat", fname)) {
      handle_string_constant(args, 0);
      handle_string_constant(args, 2);

      rs->check_strcat((char*) args[0], (const char*) args[2]);
    } else if ( 0 == strcmp("strncat", fname)) {
      rs->check_strncat( (char*) args[0], (const char*) args[2], num_arg(args, argc, 4));
    } else if ( 0 == strcmp("strchr", fname)) {
      rs->check_strchr((const char*) args[0], num_arg(args, argc, 2));
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

  rs -> checkpoint( SourcePosition(si) );

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
    } else if ( 0 == strcmp("std::fstream", fname)) {
			 rs -> checkFileAccess((std::fstream&) file, strcmp(arg1,"r") == 0 /* is_read? */);
    } else {
			// \pp does a fall through indicate an error?
		}
}


// ***************************************** FUNCTION CALL *************************************



// ***************************************** SCOPE HANDLING *************************************

void rted_EnterScope(const char* name)
{
	rted_ProcessMsg();

  RuntimeSystem * rs = RuntimeSystem::instance();
  rs -> beginScope( name );
}

void rted_ExitScope(const char*, SourceInfo si)
{
	rted_ProcessMsg();

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
int rted_CreateVariable( TypeDesc td,
												 Address address,
												 size_t size,
												 const char* name,
												 const char* mangled_name,
												 int init,
												 const char* class_name,
												 SourceInfo si
						           )
{
	rted_ProcessMsg();

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
      rs -> checkMemWrite( address, td.desc, size );
  }

  // can be invoked as part of an expression
  return 0;
}


int rted_CreateObject( TypeDesc td, Address address, SourceInfo si )
{
	rted_ProcessMsg();

	assert(td.desc.shared_mask == 0); // no objects in UPC

  RuntimeSystem * rs = RuntimeSystem::instance();
  rs -> checkpoint( SourcePosition( si ));

	RsType*         rs_type = rs_getType(*rs->getTypeSystem(), td.name, td.base, "", td.desc);
  RsClassType*    rs_classtype = static_cast< RsClassType* >(rs_type);

	assert(rs_classtype);
  rs->createObject( address, rs_classtype );

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
											 rted_AllocKind  allocKind,
											 const char*     class_name,
											 rted_SourceInfo si
										 )
{
  rted_ProcessMsg();
	snd_InitVariable         ( td, address, size, pointer_changed, allocKind, class_name, si );

	return _rted_InitVariable( td, address, size, pointer_changed, allocKind, class_name, si );
}


int _rted_InitVariable( rted_TypeDesc   td,
		                    rted_Address    address,
											  size_t          size,
											  int             pointer_changed,
											  rted_AllocKind  allocKind,
											  const char*     class_name,
											  rted_SourceInfo si
										  )
{
  RuntimeSystem * rs = RuntimeSystem::instance();

  rs -> checkpoint( SourcePosition( si ));

	std::stringstream message;

	message << "   Init Var at address:  " << HexToString(address.local)
					<< "  type:" << td.name
					<< "   size: " << ToString(size);
  rs->printMessage(message.str());

  RsType* rs_type = rs_getType(*rs->getTypeSystem(), td.name, td.base, class_name, td.desc);
  rs -> checkMemWrite( address, td.desc, size, rs_type );

  // This assumes roseInitVariable is called after the assignment has taken
  // place (otherwise we wouldn't get the new heap address).
  //
  // Note that we cannot call registerPointerChange until after the memory
  // creation is registered, which is done in roseCreateHeap.

  if (  (allocKind & akCxxHeap) == akCxxHeap
	   && pointer_changed == 1
	   && strcmp( "SgPointerType", td.name) == 0
		 )
	{
    Address        heap_address = rted_deref(address, td.desc);
		AddressDesc    heap_desc = rted_deref_desc(td.desc);
		RsPointerType* rp = dynamic_cast<RsPointerType*>(rs_type);

    assert(rp);
    rs->registerPointerChange( address, td.desc, heap_address, heap_desc, rp, false, true );
  }

  // can be invoked as part of an expression
  return 0;
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

  RuntimeSystem * rs = RuntimeSystem::instance();
  rs -> checkpoint( SourcePosition( si ));

  Address        heap_address = rted_deref(address, td.desc);
	AddressDesc    heap_desc = rted_deref_desc(td.desc);
  RsType*        rs_type = rs_getType(*rs->getTypeSystem(), td.name, td.base, class_name, td.desc);
	RsPointerType* rp = dynamic_cast<RsPointerType*>(rs_type);

  rs->registerPointerChange( address, td.desc, heap_address, heap_desc, rp, true, false );
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
	rted_ProcessMsg();

  RuntimeSystem * rs = RuntimeSystem::instance();
  rs->checkpoint( SourcePosition( si ));

	rs_checkMemoryAccess(*rs, address, rted_obj(), size, read_write_mask & Read);
  rs_checkMemoryAccess(*rs, write_address, rted_obj(), write_size, read_write_mask & Write);
}

// ***************************************** VARIABLE DECLARATIONS *************************************


// A simple way for users to manually set checkpoints
void rted_Checkpoint(SourceInfo si)
{
	rted_ProcessMsg();

  RuntimeSystem * rs = RuntimeSystem::instance();
  rs -> checkpoint( SourcePosition( si ));
}

void rted_RegisterTypeCall( const char* nameC,
                            const char* /* typeC */,
														const char* isUnionType,
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
	RuntimeSystem* rs = RuntimeSystem::instance();
	TypeSystem&    ts = *rs->getTypeSystem();
	bool           isUnion = (*isUnionType=='1');

	va_start(vl, argc);

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

	size_t          argctr = 0;

	while (argctr < argc)
	{
		  argctr += 4;

			assert(argctr <= argc);

		  RsType*     t = NULL;
      std::string name(va_arg(vl,const char*));
			TypeDesc    td = va_arg(vl,TypeDesc);
      size_t      offset = va_arg(vl, size_t);
      size_t      size = va_arg(vl, size_t);

			if (td.name == std::string("SgArrayType"))
			{
				// \pp was: t = RuntimeSystem_getRsArrayType(ts, &vl, dimensionality, size, base_type );

        assert(argctr < argc);
				const size_t dimensionality = va_arg( vl, size_t );
				assert(dimensionality > 0);

				size_t       dims[dimensionality+1];
				size_t       pos = 0;

				dims[pos] = dimensionality;
        argctr += dimensionality + 1;  // adjust the loop counter
				assert(argctr <= argc);

				while (pos < dimensionality)
				{
					dims[++pos] = va_arg( vl, size_t );
			  }

				t = rs_getArrayType(ts, dims, size, td.base);
      }
			else
			{
        t = rs_getType(ts, td.name, td.base, "", td.desc);
      }

      assert(t != NULL);
      std::stringstream message;

			 message << "   Register class-member:  " << name
			         << "  offset:" << HexToString(offset)
							 << "   size: " + ToString(size);

			rs->printMessage(message.str());
      classType->addMember(name, t, offset);

      //cerr << "Registering Member " << name << " of type " << type << " at offset " << offset << endl;
  }

	va_end(vl);
}

void _rted_FreeMemory(rted_Address addr, rted_AllocKind freeKind, rted_SourceInfo si)
{
  RuntimeSystem * rs = RuntimeSystem::instance();

	rs->checkpoint( SourcePosition(si) );
	rs->freeMemory( addr, freeKind );
}

void rted_FreeMemory(rted_Address addr, rted_AllocKind freeKind, rted_SourceInfo si)
{
	rted_ProcessMsg();
	snd_FreeMemory( addr, freeKind, si );

	_rted_FreeMemory( addr, freeKind, si );
}

void rted_ReallocateMemory( void* ptr, size_t size, SourceInfo si )
{
	rted_ProcessMsg();

  RuntimeSystem * rs = RuntimeSystem::instance();
  rs->checkpoint( SourcePosition(si) );

	rs->freeMemory( memAddr(ptr), akCHeap );
	rs->createMemory( memAddr(ptr), size, akCHeap );
}


void rted_CheckIfThisNULL( void* thisExp, SourceInfo si)
{
	rted_ProcessMsg();

	RuntimeSystem * rs = RuntimeSystem::instance();

	rs->checkpoint( SourcePosition( si ));
	rs->checkIfThisisNULL(thisExp);
}
