%module(directors="1") sage3
#pragma SWIG nowarn=516

class SgNode;
%typemap(javaout) SgNode *, SgNode const *, SgNode const*& %{{return gov.llnl.casc.rose.MakeSgNode.makeDerivedJavaNodeClass($jnicall, $owner);}%}

%include <various.i>
%include <std_vector.i>
%include <std_except.i>
%include <std_list.i>
%include <std_pair.i>
%include <std_string.i>
%include <std_common.i>
%include <typemaps.i>

%include "wchart.i"
%include "sgname.i"

#define Rose_STL_Container std::vector

%define SG_NODE_BODY(TYPENAME)
%typemap(javabody) TYPENAME  %{
  protected boolean swigCMemOwn;
  private long swigCPtr;
  /*protected*/ public Ptr links = null;
  private static java.util.concurrent.locks.ReentrantLock new_delete_lock
    = new java.util.concurrent.locks.ReentrantLock();

  static private int lastid = 0;
  protected int id;

  public void updateLinks() {
    if (links != null) {
      Ptr.setLinksFor(this, links);
    }
  }

  private static long lock_new_delete() {
    new_delete_lock.lock();
    return 0;
  }

  private static long unlock_new_delete() {
    new_delete_lock.unlock();
    return 0;
  }

  public String toString() {
    return getClass().getName() + ":" + id;
  }

  protected $javaclassname(long cPtr, boolean cMemoryOwn) {
    id = lastid++;
    if (cPtr == 0) throw new NullPointerException();
    if (cMemoryOwn) {
      //System.err.println("Building a " + this.getClass().getName());
      Log.log.println("Building: " + this.getClass().getName() + " at " + cPtr);
    }
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
    links = Ptr.createPtr(cPtr);
//    if (cMemoryOwn)
      links.addSource(this);
    updateLinks();
  }

  public static long getCPtr($javaclassname obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  public void force_mgmt() {
    swigCMemOwn = true;
  }

  public void force_nomgmt() {
    swigCMemOwn = false;
  }
%}

%typemap(javabody_derived) TYPENAME %{
  private long swigCPtr;
  private static java.util.concurrent.locks.ReentrantLock new_delete_lock
    = new java.util.concurrent.locks.ReentrantLock();

  private static long lock_new_delete() {
    new_delete_lock.lock();
    return 0;
  }

  private static long unlock_new_delete() {
    new_delete_lock.unlock();
    return 0;
  }

  protected $javaclassname(long cPtr, boolean cMemoryOwn) {
    super($imclassname.SWIG$javaclassnameUpcast(cPtr), cMemoryOwn);
    swigCPtr = cPtr;
    if (getClass().equals(SgFile.class)) {
      System.err.print("Creating (internal) ");
      System.err.println(this);
    }
  }

  public static long getCPtr($javaclassname obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

%}

%typemap(javadestruct, methodname="delete", methodmodifiers="public synchronized") TYPENAME {
    if(swigCPtr != 0 && swigCMemOwn) {
      //Ptr.debugGraph(links);
      swigCMemOwn = false;
      lock_new_delete();
      try {
	//System.err.println("Deleting $javaclassname");
        Log.log.println("Deleting: " + this.getClass().getName() + " at " + swigCPtr);

        $jnicall;
      } finally {
        unlock_new_delete();
      }
    }
    if (links != null)
      links.removeSource(this);
    swigCPtr = 0;
  }

%typemap(javadestruct_derived, methodname="delete", methodmodifiers="public synchronized") TYPENAME {
    if(swigCPtr != 0 && swigCMemOwn) {
      //Ptr.debugGraph(links);
      swigCMemOwn = false;
      lock_new_delete();
      try {
	//System.err.println("Deleting $javaclassname");
        Log.log.println("Deleting: " + this.getClass().getName() + " at " + swigCPtr);

        $jnicall;
      } finally {
        unlock_new_delete();
      }
    }
    if (links != null)
      links.removeSource(this);
    swigCPtr = 0;
    super.delete();
  }

%typemap(javaconstruct, directorconnect="\n    $imclassname.$javaclassname_director_connect(this, swigCPtr, swigCMemOwn, true);") TYPENAME {
    this(lock_new_delete() + $imcall + unlock_new_delete(), true);
    $directorconnect
  }

%enddef

SG_NODE_BODY(SWIGTYPE)

%{
class Sg_Options {} __sg_opt_a;
Sg_Options& Sg_options()
{
  return __sg_opt_a;
}

#include "rose.h"
#include "sage3.h"
// #include "AstFixParentTraversal.h"
#include "compass.h"

void buildCheckers( std::vector<const Compass::Checker*> & checkers, Compass::Parameters & params, Compass::OutputObject & output, SgProject* proj );

typedef std::vector<SgNode*> AstNodePointersList;

// extern "C" {
//   extern void data_state_save();
//   extern void data_state_reset();
// }

%}

namespace __gnu_cxx {}

%apply char **STRING_ARRAY { char **argv };

// namespace std {

#define GEN_TYPE(T, VT, JT, VJT)					\
   %typemap(javainterfaces) std::VT<T*> "GenericVector<" #T ">"		\
   %template(VJT) std::VT<T*>;

#define VECT_TYPE(T)   GEN_TYPE(T, vector, T, T##Vector)
// #define LIST_TYPE(T)   GEN_TYPE(T, list, T, T##List)

   VECT_TYPE(SgNode)
   VECT_TYPE(SgFile)
   VECT_TYPE(SgDeclarationStatement)
   VECT_TYPE(SgType)
   VECT_TYPE(SgStatement)
   VECT_TYPE(SgInitializedName)
   VECT_TYPE(SgBaseClass)
   VECT_TYPE(SgDirectory)
   VECT_TYPE(SgExpression)
   // LIST_TYPE(SgTemplateArgument)
   VECT_TYPE(SgQualifiedName)
   VECT_TYPE(SgModifierType)

   %template(unsignedVector) std::vector<unsigned long>;

   %template(SgNodeStringPair) std::pair<SgNode*,std::string>;
   %typemap(javainterfaces) std::vector<std::pair<SgNode*,std::string> >
      "GenericVector<SgNodeStringPair>"
   %template(SgNodeStringPairVector) std::vector<std::pair<SgNode*,std::string> >;

   %template(AttachedPreprocessingInfoType) std::list<PreprocessingInfo*>;
// }

%ignore *::operator=;
%ignore *::operator new;
%ignore *::operator delete;
%ignore *::operator <<;
%ignore *::operator <;
%ignore *::operator ();
%ignore *::operator +;
%ignore operator +;
%ignore *::operator +=;
%ignore *::operator !=;
%ignore operator <<;
%rename(equals) *::operator==;
%ignore operator==;
%ignore operator!=;
%ignore operator<;
%ignore operator>;
%ignore operator<=;
%ignore operator>=;
%rename(toString) operator std::string;

%feature("director") ROSE_VisitTraversal;
%feature("director") Compass::OutputObject;

%include sage3.h
%include utility_functions.h
%include Cxx_Grammar.i
%include AstProcessing.h
%include AstSimpleProcessing.h
%include AstConsistencyTests.h
%include unparser.h
%include rose_attributes_list.h
%include AstProcessing.h
// %template(AstTDP_ParentAttr) AstTopDownProcessing<ParentAttribute>;
// %include AstFixParentTraversal.h
%include AST_FILE_IO.h
%include astQueryInheritedAttribute.h
%include astQuery.h
%include nodeQuery.h
%include compass.h

%template(CheckerVector) std::vector<const Compass::Checker*>;
%typemap(javainterfaces) std::vector<const Compass::Checker*> "GenericVector<Checker>"

%template(OutputObjectVector) std::vector<Compass::OutputObject*>;
%typemap(javainterfaces) std::vector<Compass::OutputObject*> "GenericVector<OutputObject>"

void buildCheckers( std::vector<const Compass::Checker*> & checkers, Compass::Parameters & params, Compass::OutputObject & output, SgProject* proj );

%{
void buildCheckers( std::vector<const Compass::Checker*> & checkers, Compass::Parameters & params, Compass::OutputObject & output, SgProject* proj );
%}

%include "casts.i"

%extend SgNode {
    std::vector<unsigned long> get_graph() {
	std::vector<unsigned long> ret;
	typedef std::vector<std::pair<SgNode*, std::string> > t;
        t v = $self->returnDataMemberPointers();
	for (t::iterator i = v.begin(); i != v.end(); i++) {
	  ret.push_back((unsigned long)((*i).first));
        }
	return ret;
    }
}

// extern "C" {
//   extern void data_state_save();
//   extern void data_state_reset();
// }
