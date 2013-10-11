/* 
                        Binary Analysis Attributes

   This file presents structures that are added as AST attributes on the AST
as a way to accumulate locally derived analysis results that can then be used
to feed less local analysis.

   In some cases the local analysis represented by specific analysis attributes
can be used to drive the generation of source code from the binary.  This is
the subject of currently work to see now far this can be extended. Currently the
focus is supporting the code generation required to support debugging the detection
of data structure use in the binary.


Design ideas:

  1) We might want to build a base class common to the sorts of classes defined below.
     This possible refactoing can be done later if it is a good idea.

  2) Some of these classes might make since as IR nodes in an alternative 
     higher level representation of the binary AST.

  3) We are using the SgAsmDataStructureDeclaration IR nodes in the work below.
     This IR node is not built into the AST within the disassemble and is only
     constructed as part of an analysis of the data structures of the binary.
     There may be more such AsmDeclaration kinds of IR nodes that we might want 
     to use, for example:
     a) SgAsmVariableDeclaration
     b) SgAsmEnumDeclaration
     c) SgAsmGlobalScope

*/

// Forward class declaration.
class VirtualTableSection;

// Forward class declaration.
class VirtualFunctionTable;


class TypeAnalysisAttribute
   {
  // This is used to hold information about variables declared on the stack or in global scope.

     public:
          std::string name;
          size_t address;
          SgAsmType* asmType;

          TypeAnalysisAttribute(size_t addr, SgAsmType* type);
   };

class ScopeAnalysisAttribute : public AstAttribute
   {
     public:
      //! Used to generate unique names for variables (stores id's given to previously referenced variables.
          std::map<size_t,int> usedScopeOffsets;
          size_t variableCounter;

          ScopeAnalysisAttribute() : variableCounter(0) {}
   };

// DQ (8/30/2013): This name was changed in 2011, so we need a typedef to reuse the older name in this code.
typedef SgAsmFunction SgAsmFunctionDeclaration;

class GlobalScopeAnalysisAttribute : public ScopeAnalysisAttribute
   {
  // This class is appended to selected SgAsmInterpretation in the binary AST (is this the best place?).
  // This is where a number of details about the global analysis of the binary can be accumulated:
  //    1) Imported functions
  //    2) dynamically loaded libraries
  //    3) results of FLIRT
  //    4) etc.

     public:
      //! Pointer to the virtual function table if this is a member function.
       // VirtualTableSection* associatedVirtualTableSection;

      //! List of virtual function tables identified in the binary (in section ".rodata")
          std::vector<VirtualFunctionTable*> virtualFunctionTableList;

       // Unclear if this name is useful (might be for multiple SgAsmInterpretation 
       // objects in a binary, 16-bit, 32-bit, 64-bit, fat-files, etc.).
          std::string name;

       // Save a reference to the SgAsmInterpretation, where this attribute is attached.
          SgAsmInterpretation* asmInterpretation;

      //! Pointer to the top level of the AST (useful to support global analysis).
          SgProject* project;

      //! Pointer to the generated SgSourceFile where generated code will be built.
          SgSourceFile* generatedSourceFile;

      //! This is the list of declarations obtained via an analysis of the binary.
      //! (global variables, data-structures, member functions, nested functions (GNU), etc.)
          std::vector<SgAsmSynthesizedDeclaration*> associatedDeclarationList;

      //! Container of referenced variables (with type information) with offsets on the stack.
          std::vector<TypeAnalysisAttribute*> stackTypeList;

       // Used to generate unique names for variables (stores id's given to previously referenced variables.
       // std::map<size_t,int> usedGlobalOffsets;
       // size_t globalCounter;

          GlobalScopeAnalysisAttribute ( std::string name, SgAsmInterpretation* s, SgProject* p );

          bool isVirtualMemberFunction ( SgAsmFunctionDeclaration* asmFunction );

          VirtualFunctionTable* associatedVirtualFunctionTable ( SgAsmFunctionDeclaration* asmFunction );
   };


class FunctionAnalysisAttribute : public ScopeAnalysisAttribute
   {
  // This class is appended to selected SgAsmFunctionDeclaration objects in the binary AST.
  // Some of this data might be put into the SgAsmFunctionDeclaration and if 
  // so then we might not need to use the AST attribute mechanism.

     public:
       // typedef to represent the address of the function in the binary.
          typedef size_t FunctionAddress;

          std::string name;

      //! Pointer to the function declaration IR node in the AST.
          SgAsmFunctionDeclaration* function;

      //! Pointer to the top level of the AST (useful to support global analysis).
          SgProject* project;

      //! Pointer to the virtual function table if this is a member function.
       // VirtualFunctionTable* associatedVirtualTable;

      //! This is the list of declarations obtained via an analysis of the binary 
      //! (stack variables, data-structures, member functions, nested functions (GNU), etc.)
          std::vector<SgAsmSynthesizedDeclaration*> associatedDeclarationList;

      //! Container of referenced variables (with type information) with offsets on the stack.
          std::vector<TypeAnalysisAttribute*> stackTypeList;

       // Used to generate unique names for variables (stores id's given to previously referenced variables.
       // std::map<size_t,int> usedStackOffsets;
       // size_t stackCounter;

          FunctionAnalysisAttribute ( std::string name, SgAsmFunctionDeclaration* f, SgProject* p );
   };


class MemberFunctionAnalysisAttribute : public FunctionAnalysisAttribute
   {
  // This class is appended to selected SgAsmFunctionDeclaration objects in the binary AST.
  // A SgAsmFunctionDeclaration is either a member function or a regular function.

     public:
      //! Pointer to the virtual function table if this is a member function.
          VirtualFunctionTable* associatedVirtualTable;

      //! This is a IR node that is build only via an analysis.
          SgAsmSynthesizedDataStructureDeclaration* associatedDataStructureDeclaration;

          MemberFunctionAnalysisAttribute ( std::string name, SgAsmFunctionDeclaration* f, SgProject* p );
   };


class VirtualFunction
   {
  // This class use used to represent each virtual function.
  // It is built from an analysis of the virtual function table for each class 
  // that has a virtual function.  If when er generate the call tree of
  // each function we call (with the correct signature) other virtual
  // functions then those are the base class constructors.

     public:
          std::string name;
          size_t virtualFunctionTableEntry;
          size_t virtualFunctionAddress;

         VirtualFunction (std::string n, size_t vFunctionAddress );
   };

   
class VirtualFunctionTable
   {
  // This class is  used in a list in the VirtualTableSection attribute
  // to represent each virtual function table.

     public:
       // This is the offset into the mapped memory of the VirtualTableSection
          size_t virtualFunctionTablePosition;

       // Allow the assignment of a generated name to the virtual function table
          std::string name;

       // Store data about each virtual function.
          std::vector<VirtualFunction*> virtualFunctionList;

       // This is the associated data structure declaration (built as part of analysis)
          SgAsmSynthesizedDataStructureDeclaration* associatedDataStructureDeclaration;

       // Not clear now to compute the size of the virtual function table 
       // (except to arrange them contiguous in memory and compute the size 
       // based on the distance to the next table). And the boundary of the 
       // section to compute the size of the last table.
          size_t size;

          VirtualFunctionTable ( SgProject* project, SgAsmElfSection* virtualFunctionTable, size_t vTableAddress );

          bool isVirtualMemberFunction ( SgAsmFunctionDeclaration* asmFunction );

          void display(const std::string & s);
   };


class VirtualTableSection : public AstAttribute
   {
  // This class is appended to the section in the AST where all virtual function 
  // tables are kept (".rodata").

     public:
       // typedef to represent the address of a virtual function table
          typedef size_t VirtualFunctionTableAddress;

          SgAsmElfSection* virtualFunctionTable;
          SgProject* project;

          std::vector<VirtualFunctionTable*> virtualFunctionTableList;

          VirtualTableSection ( SgAsmElfSection* vTable, SgProject* p );

#if 0
          void printOutVirtualFunctionTableInformation ( VirtualFunctionTableAddress vTable );
#endif
          bool tableExists ( VirtualFunctionTableAddress vTable );

       // This should be called only once per virtual function table.
          void buildVirtualFunctionTable ( VirtualFunctionTableAddress vTable );
   };


class VirtualTableLoad : public AstAttribute
   {
  // This class is appended to the AST as an attribute where a virtual tabel load is detected.
  // it is designed to hold easily accessible relevant date for use in subsequent analysis.

     public:
       // rose_addr_t virtualTableAddress;
       // SgAsmMemoryReferenceExpression* 
          SgAsmValueExpression* virtualTableAddress;

       // This is initialized after all the VirtualTableLoad objects are built.
          VirtualFunctionTable* associatedVirtualTable;

          VirtualTableLoad ( SgAsmInstruction* instruction );
   };


class DataMemberInitializationAttribute : public AstAttribute
   {
  // This class is appended to the SgAsmInstructions representing memory loads in the binary AST.
  // Then an analysis can classify the type of memory load.

     public:
       // Not clear if we should store the function.
          SgAsmFunctionDeclaration* function;

      //! This is the reference to the instructon where this attribute is attached.
          SgAsmInstruction* instruction;

      //! This is the top level of the AST (used to support global analysis).
          SgProject* project;

       //! This is the offset of the data access within the struct/class, global scope or stack.
          size_t offset;

       //! This is the value being loaded into memory.
          size_t value;

       //! This is the type as held in the opcode.
          SgAsmType* type;

      //! Record if this is a stack variable refernece, else assume it is a global variable.
          bool isStackVariable;

      //! this is the value of the eax register (a pointer to the struct/class data layout mapped into memory).
          size_t this_pointer_value;

      //! This is the class for which this is a data member initialization (built as part of the data structure analysis).
          SgAsmSynthesizedDataStructureDeclaration* associatedDataStructureDeclaration;

       // This is the associated field value in the class.
          SgAsmSynthesizedFieldDeclaration* fieldDeclaration;

          DataMemberInitializationAttribute ( SgAsmInstruction* instruction, SgProject* p );
   };

