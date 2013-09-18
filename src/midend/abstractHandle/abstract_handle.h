/*!
 * A reference implementation of abstract handles for language constructs
 * Used for specifying a unique language construct in source files.
 *
 * Users are required to implement the interface using their own
 * IR node types.
 *
 * Goal : 
 *     Only define interfaces and abstract implementation
 *     independent to any particular compilers or tools
 */
#ifndef abstract_handle_INCLUDED
#define abstract_handle_INCLUDED

#include <string>
#include <utility>
#include <limits.h>
#include <map>
#include "rosedll.h"

#ifdef _MSC_VER
// DQ (11/26/2009): Required for definition of PATH_MAX in MSVC.
#include "rose_msvc.h"
#endif

namespace AbstractHandle{

  //source position information:
  struct source_position
  {
    size_t line;
    size_t column;
  };

  struct source_position_pair
  {
    source_position first;
    source_position second;
  };

  std::string toString(const source_position &);
  std::string toString(const source_position_pair &);
  bool isEqual(const source_position &pos1, const source_position &pos2);
  bool isEqual(const source_position_pair &pair1, const source_position_pair &pair2);
   
  // Parse a string (such as 12.10, 13, 0 etc )into source position data structure
  void fromString(source_position&, const std::string& input);
  // Parse a string into source position pair data structure
  void fromString(source_position_pair&, const std::string& input);

 //----------specifiers------------------------------------
  // specifier type and values
  typedef  enum {e_name, e_position, e_numbering, e_int_label, e_str_label} 
    specifier_type_t;

  typedef union u_specifier_value
     {
       char str_v[PATH_MAX+256];
       size_t int_v; 
       source_position_pair positions;
     } specifier_value_t;

  // construct specifier 
  // could be used to specify a construct by name, position, numbering, labels, etc
  // (e_name, foo) (e_position, 15)
  class specifier
  {
  public:
    specifier(){};
    specifier (specifier_type_t t, specifier_value_t v):type(t),value(v){}
    specifier_type_t get_type(){return type;}
    specifier_value_t  get_value(){return value;}
    void set_type(specifier_type_t stype) { type=stype;}
    void set_value(specifier_value_t svalue) {value=svalue;}
    std::string toString();
  private:  
     specifier_type_t type; 
     specifier_value_t value ;
  };

  // Parse a string into a specifier 
  void fromString(specifier&, const std::string& input);

 //----------------Abstract AST/IR node---------------
 //Users  should provide a concrete node implementation
 // especially a constructor/builder to avoid duplicated creation of abstract_node.
 // We try not to use pure virtual functions here to allow 
 // users to reuse the default implementation as much as possible
 class abstract_node
 {
  public: 
  virtual ~abstract_node(){}

  // Get the construct' s type name, like function
  virtual std::string getConstructTypeName() const;

  //If the node has meaningful line and column numbers associated with a file
  virtual bool hasSourcePos() const;

  //If the node has legal names defined by language standards
  virtual bool hasName() const;

  // Get the name of the construct if it is named, like function name
  virtual std::string getName() const;

  //Get the start source file position of the construct
  // Get the abstract node for file containing the current node.
  virtual abstract_node* getFileNode() const;

  //Get parent node, used for generate parent handle automatically
  virtual abstract_node* getParent() const;

#ifdef _MSC_VER
// DQ (11/27/2009): MSVC does not appear to support covariant types, but no message is required.
 #pragma message ("WARNING: MSVC does not handle covariant return types properly.")
#else
  //Get the raw IR node associated with the current abstract node
  virtual void * getNode() const {return NULL;};
#endif

  //Find a node from a string for a abstract handle's string format, starting from this node
  // eg. find a file node from a string like SourceFile<name,/home/liao6/names.cpp>
  virtual abstract_node* findNode(std::string handle_str) const;  

  //Find a node of a given type, it also matches the specifier.
  virtual abstract_node* findNode(std::string construct_type_str, specifier mspecifier) const;  

  virtual std::string getFileName() const;

  virtual source_position_pair getSourcePos() const;
  virtual source_position getStartPos() const;
  virtual source_position getEndPos() const;

  virtual std::string toString() const;

  //Get integer label
  virtual size_t getIntLabel() const;
  //Get string label
  virtual std::string getStringLabel() const;

  //Get the ordering of the construct relative to another construct in a higher scope
  // Numbering start from 1
  //e.g. a function construct may be the 2nd function relative to a file
  virtual size_t getNumbering( const abstract_node*  another_node) const;

  virtual bool operator == (const abstract_node & x) const=0;
 }; // end class abstract_node

  //---------------- An abstract handle --------------------------
  // to specify a construct using a specifier
  // Can be used alone or with parent handles when relative specifiers are used
  class ROSE_DLL_API abstract_handle 
  {
  public:
    //Constructors
    
    abstract_handle();
    // Create a handle from a node, using the source position as the specifier by default
    // Or use name if source position is not available, then use numbering as the final resort.
    abstract_handle(abstract_node* node);

    // Create a handle from an optional parent handle, 
    // with explicit type and numbering information available.
    //It also tries its best to recursively generate parent handles using the given specifier type 
    //if parent_handle is not provided in the parameter list. 
    abstract_handle(abstract_node* node, specifier_type_t stype, \
                   specifier_value_t svalue, abstract_handle* p_handle=NULL);

    //Create a handle using specified type, automatically fill out the corresponding value
    //Most useful to create a numbering or name specifier within another handle
    //It also tries its best to recursively generate parent handles using the given specifier type 
    //if parent_handle is not provided in the parameter list. 
     abstract_handle(abstract_node* node, specifier_type_t stype, \
               abstract_handle* p_handle=NULL);

    //construct a handle from a handle string  within the scope of an existing handle
    //e.g: <SourceFile<name,/home/liao6/names.cpp> will create a corresponding file handle 
    abstract_handle(abstract_handle* phandle, const std::string& handle_string);
    
    virtual ~abstract_handle() { if (m_specifier !=NULL) delete m_specifier;}

    // the language construct type name
    virtual std::string get_construct_type_name();

    // Output the handle to a string, including all parent handles
    virtual std::string toString();

    // Initialize a handle from a handle string, generate full parent handles when necessary
    virtual void fromString(abstract_handle* ancester_handle, const std::string &handle_str_multiple);

    // Only output the handle itself to a string, excluding parent handles
    virtual std::string toStringSelf();

    //Instantiate a handle using a handle element's string (a single handle only),the handle refers to a node under a parent
    virtual void fromStringSelf(abstract_handle* p_handle, const std::string& handle_str_item);

   virtual abstract_node* getNode() const {return m_node;}
  // allow multiple specifiers or not?
   specifier* get_specifier(){return m_specifier;}
  
  abstract_handle* get_parent_handle(){return parent_handle; }
  void set_parent_handle(abstract_handle* parent){ parent_handle= parent;}

  // operator ==
  //Any other way to implement this??
  bool operator == (const abstract_handle& x) const
    { return m_node == x.getNode();}

  private: 
    // initialize all internal members to NULL
    void init();
    abstract_handle * parent_handle;
    // shared handles cannot have a single next handle
    // abstract_handle * next_handle; // expose them or not?
    
    // associated IR node 
    abstract_node* m_node; 
    specifier * m_specifier;
    abstract_handle (const abstract_handle &); // disallow copy
    abstract_handle & operator = (const abstract_handle &);

  };

  // maintain a map between nodes and handles, 
  // used for fast indexing and avoid redundant creation of handles for nodes
  // Should update the map after each creation
 extern std::map<abstract_node*, abstract_handle*> handle_map; 

}// end namespace 

#endif 
