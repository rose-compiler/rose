#ifndef COMPASS_CHECKNAMEIMPL_H
#define COMPASS_CHECKNAMEIMPL_H






#if USE_ROSE_BOOST_WAVE_SUPPORT //GMY 12/26/2007 added #if...#endif to pass compass make verify
#include <boost/wave.hpp>

#include <boost/regex.hpp>
#endif //GMY 12/26/2007 END
#include <functional>
#include <fstream>


enum name_types {

     FIRST_ELEMENT_TYPE ,
     // functions
     function,
     static_function    ,
     member_function    ,
     global_function    ,
     static_global_function ,
     static_member_function ,
     // variables

     class_field        ,
     ptr_class_field    ,
     ref_class_field    ,
     static_class_field     ,
     static_ptr_class_field ,
     static_ref_class_field ,

     stack_variable     ,
     ptr_stack_variable  ,
     ref_stack_variable ,
     static_stack_variable ,
     static_ptr_stack_variable ,
     static_ref_stack_variable ,

     global_variable        ,
     ptr_global_variable    ,
     ref_global_variable    ,
     static_global_variable ,
     static_ptr_global_variable ,
     static_ref_global_variable ,
     
     variable               ,
     ptr_variable           ,
     ref_variable           ,
     static_variable       ,
     static_ptr_variable    ,
     static_ref_variable    ,

     bool_type_variable,
     
     method_argument_name ,
     ptr_method_argument_name ,
     ref_method_argument_name ,
     static_method_argument_name   ,
     static_ptr_method_argument_name ,
     static_ref_method_argument_name,
     
     global_constant   ,
     //class names      
     class_name    , 
     //misc    
     typedef_name,
     macro_name,      
     enum_name,
     enum_label,
     enum_names_prefix,

     //filtering mechanism
     filter_namespace,
     keep_only_namespace,
     filter_path,
     filter_path_reg,
     keep_only_path,
     keep_only_path_reg,
     LAST_ELEMENT_TYPE,

};


typedef std::vector<SgNode*> SgNodePtrVector;


class NameEnforcer{

     public:
	  void enforceRules(SgNode*, std::list< std::pair<name_types,SgNode*> >& violations,
                               std::list< std::pair<name_types,PreprocessingInfo*> >& macroViolations  );
          void readFile( std::string filename);
          std::string get_enumName(name_types name );

          std::string get_reg(name_types enum_elem);


	  NameEnforcer();	  
     private:
	  void Initialize(); 
          //map which translates between the string and the enums representing the strings
	  std::map<std::string, name_types> s_mapStringValues;
          //map which contains the regular expresssions which is defined
	  std::map<name_types, std::string> s_definedRegularExpressions;

	  //stdext::hash_multimap<name_types,std::string> filterPathMap;
	  std::multimap<name_types,std::string> filterPathMap;
	  std::list<std::string> namespacesToFilter;
	  std::list<std::string> namespacesToKeep;

          std::list<SgNode*> removeElements(std::list<SgNode*>& nodeList);

	  
	  bool def_reg( name_types enum_elem  );
	  std::pair<std::string,name_types> defaultVariableDeclaration(SgType* varType, bool isStaticVariable);
	  void checkVariableDeclaration(SgVariableDeclaration*, std::list< std::pair<name_types,SgNode*> >&);
	  void checkFunctionDeclaration(SgFunctionDeclaration*, std::list< std::pair<name_types,SgNode*> >&);
	  void checkClassDeclaration(SgClassDeclaration*, std::list< std::pair<name_types,SgNode*> >&);
	  void checkMacroNames(PreprocessingInfo*, std::list< std::pair<name_types,PreprocessingInfo*> >&);
          void checkEnumDeclaration(SgEnumDeclaration*, std::list< std::pair<name_types,SgNode*> >&);
          void checkEnumLabels(SgEnumDeclaration*, std::list< std::pair<name_types,SgNode*> >&);

          std::vector< std::pair<SgNode*,std::string> > findUses(SgLocatedNode* locNode);


};



/*******************************************************************************************************************
 * The binary functional
 *       struct filterOnEdges
 * returns an AST_Graph::FunctionalReturnType and takes a node and a std::pair<SgNode*,std::string> as a paramater.
 * The paramater represents an edge 'SgNode*'->'std::pair<>' where the pair is a variable declaration with name 
 * 'std::string' within the node in the first paramater. 
 *
 * The type AST_Graph::FunctionalReturnType contains the variables
 *      * addToGraph : if false do not graph node or edge, else graph
 *      * DOTOptions : a std::string which sets the color of the node etc.
 *******************************************************************************************************************/
struct filterOnPaths: public std::binary_function< SgNode*,std::multimap<name_types,std::string>&,std::list<SgNode*> >
   {
  // This functor filters SgFileInfo objects and IR nodes from the GNU compatability file
     result_type operator() ( SgNode*, std::multimap<name_types,std::string>&) const;
   };




// Build a synthesized attribute for the tree traversal
class SynthesizedAttribute
   {
     public:
       // List of #define directives (save the PreprocessingInfo objects
       // so that we have all the source code position information).
          std::list<PreprocessingInfo*> accumulatedList;

          void display() const;
   };


#endif
