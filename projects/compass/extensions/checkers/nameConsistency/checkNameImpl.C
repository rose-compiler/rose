#include "rose.h"




#include "checkNameImpl.h"

namespace CompassAnalyses
   {
     namespace NameConsistency
        {


//The edge goes from 'node'->'x'
filterOnPaths::result_type
filterOnPaths::operator()(SgNode* node, std::multimap<name_types, std::string>& mapOfPathsToFilter) const
   {

     std::list<SgNode*> returnValue;

      

     
     return returnValue;
   }



/*
 *  The function
 *      findScope()
 *  takes as a parameter a SgNode* which is a SgStatement*. It returns a SgNodePtrVector of all
 *  preceding scopes the SgStatement is in.
 *
 */
std::list<std::string>
findScopes (SgNode * astNode)
   {
     ROSE_ASSERT (isSgStatement (astNode));

     std::list<std::string> returnVector;
     SgScopeStatement *currentScope;

     if (isSgScopeStatement (astNode))
	{
	  currentScope = isSgScopeStatement (astNode);
	  ROSE_ASSERT (currentScope != NULL);
	}
     else
	{
	  SgStatement *sageStatement = isSgStatement (astNode);
	  ROSE_ASSERT (sageStatement != NULL);
	  currentScope = sageStatement->get_scope ();
	  ROSE_ASSERT (currentScope != NULL);
	}

     

     while (currentScope->variantT () != V_SgGlobal)
	{
	  if(isSgNamespaceDefinitionStatement(currentScope) != NULL)
	     {

	       SgNamespaceDeclarationStatement* namespaceDecl = isSgNamespaceDefinitionStatement(currentScope)->get_namespaceDeclaration();

	       returnVector.push_back (namespaceDecl->get_name().str());
	     }

	  currentScope = currentScope->get_scope ();
	  ROSE_ASSERT (currentScope != NULL);
	}



     return returnVector;
   }


NameEnforcer::NameEnforcer() : filterPathMap(), namespacesToFilter(), namespacesToKeep()
   {
     Initialize();
   }


/*******************************************************************************
  *
  * The function
  *      NameEnforcer::readFile(std::string)
  * will read the input file specified as an argument and then create a map of
  * the rules. It expects the input file to be on the form:
  *              style_key_name regular_expression
  ******************************************************************************/
void NameEnforcer::readFile( std::string filename){

     std::ifstream* file_op = Compass::openFile(filename);
     if (file_op->fail()) {
	  std::cout << "error: could not find file \"" << filename 
	           << "\" which is meant to include the styles to enforce with " 
		   << "the name checker." << std::endl;
			          exit(1);    // abort program
				      
				  }
     std::string current_word;
     bool is_first_word = true;
     name_types  cur_regexp_type;

     char dummyString[2000];
  //read file
     while( (*file_op) >> current_word){
       //First word denotes what the regular expression should operate
       //upon. Second word denotes the regular expression

	 if(  current_word.substr(0,1) == std::string("#") ){
	      //Skip rest of the line if a "#" character is found. This denotes a 
	      //comment
	      file_op->getline(dummyString,2000);
	      
	  }else if(is_first_word == true){
	       is_first_word=false;
	       if( s_mapStringValues.find(current_word) == s_mapStringValues.end() ){
		    std::cerr << "Invalid type of check \"" << current_word << "\" specified." 
			      << " Please check input file " << filename << " for errors." 
			      << std::endl;

		    exit(1);
	       }
//	       assert(s_mapStringValues.find(current_word) != s_mapStringValues.end());
	       cur_regexp_type = s_mapStringValues[current_word];
	    //std::cout << current_word << " " <<std::endl;
	  }else{
	       is_first_word=true;
	    //std::cout << current_word << std::endl;

	       switch(cur_regexp_type){
		    case filter_namespace:
		    case keep_only_namespace:
		    case filter_path:
		    case filter_path_reg:
		    case keep_only_path:
		    case keep_only_path_reg:
		    //Read in paths to filter
		       filterPathMap.insert(std::make_pair(cur_regexp_type,current_word) );
		       break;
		    default:
		    //Read in regular expression for named constucts
		       s_definedRegularExpressions[cur_regexp_type] = current_word;
		       break;
	       };

	  }
     }
   delete file_op;
}


std::string NameEnforcer::get_enumName(name_types name ){
   std::string nameStr = "";
   for(std::map<std::string, name_types>::iterator iItr = s_mapStringValues.begin();
       iItr != s_mapStringValues.end(); ++iItr ){
        if(iItr->second == name){
           nameStr = iItr->first;
           break;
        }
   }  
   ROSE_ASSERT(nameStr != "");
   return nameStr;
};


/*********************************************************************************************
  *
  * The function
  *        NameEnforcer::Initialize()
  * will initialize a map of strings which are keywords to field in the enum 'name_types' so
  * which is an equivalent data structure in this case we can more easily deal with.
  ********************************************************************************************/
void NameEnforcer::Initialize()
   {

     s_mapStringValues["FIRST_ELEMENT_TYPE"] = FIRST_ELEMENT_TYPE; 
     s_mapStringValues["function"] = function;    
     s_mapStringValues["static_function"] = static_function;    
     s_mapStringValues["member_function"] = member_function;    
     s_mapStringValues["global_function"] = global_function;    
     s_mapStringValues["static_global_function"] = static_global_function;    
     s_mapStringValues["static_member_function"] = static_member_function; 

     s_mapStringValues["class_field"] = class_field;        
     s_mapStringValues["ptr_class_field"] = ptr_class_field;    
     s_mapStringValues["ref_class_field"] = ref_class_field;    
     s_mapStringValues["static_class_field"] = static_class_field;     
     s_mapStringValues["static_ptr_class_field"] = static_ptr_class_field; 
     s_mapStringValues["static_ref_class_field"] = static_ref_class_field; 

     s_mapStringValues["stack_variable"] = stack_variable;     
     s_mapStringValues["ptr_stack_variable"] = ptr_stack_variable;  
     s_mapStringValues["ref_stack_variable"] = ref_stack_variable; 
     s_mapStringValues["static_stack_variable"] = static_stack_variable; 
     s_mapStringValues["static_ptr_stack_variable"] = static_ptr_stack_variable; 
     s_mapStringValues["static_ref_stack_variable"] = static_ref_stack_variable; 

     s_mapStringValues["global_variable"] = global_variable;        
     s_mapStringValues["ptr_global_variable"] = ptr_global_variable;    
     s_mapStringValues["ref_global_variable"] = ref_global_variable;    
     s_mapStringValues["static_global_variable"] = static_global_variable; 
     s_mapStringValues["static_ptr_global_variable"] = static_ptr_global_variable; 
     s_mapStringValues["static_ref_global_variable"] = static_ref_global_variable; 

     s_mapStringValues["variable"] = variable;               
     s_mapStringValues["ptr_variable"] = ptr_variable;           
     s_mapStringValues["ref_variable"] = ref_variable;           
     s_mapStringValues["static_variable"] = static_variable;       
     s_mapStringValues["static_ptr_variable"] = static_ptr_variable;    
     s_mapStringValues["static_ref_variable"] = static_ref_variable;    

     s_mapStringValues["bool_type_variable"] = bool_type_variable;    
     
     s_mapStringValues["method_argument_name"] = method_argument_name; 
     s_mapStringValues["ptr_method_argument_name"] = ptr_method_argument_name; 
     s_mapStringValues["ref_method_argument_name"] = ref_method_argument_name; 
     s_mapStringValues["static_method_argument_name"] = static_method_argument_name;   
     s_mapStringValues["static_ptr_method_argument_name"] = static_ptr_method_argument_name; 
     s_mapStringValues["static_ref_method_argument_name"] = static_ref_method_argument_name;

     s_mapStringValues["global_constant"] = global_constant;   
     s_mapStringValues["class_name"] = class_name;     
     s_mapStringValues["enum_name"] = enum_name;
     s_mapStringValues["enum_label"] = enum_label;

     s_mapStringValues["enum_names_prefix"] = enum_names_prefix;    
     
     s_mapStringValues["typedef_name"] = typedef_name; 
     s_mapStringValues["macro_name"] = macro_name;      

     s_mapStringValues["filter_namespace"] = filter_namespace;
     s_mapStringValues["keep_only_namespace"] = keep_only_namespace;
     s_mapStringValues["filter_path"] = filter_path;
     s_mapStringValues["filter_path_reg"] = filter_path_reg;
     s_mapStringValues["keep_only_path"] = keep_only_path; 
     s_mapStringValues["keep_only_path_reg"] = keep_only_path_reg;      

     s_mapStringValues["LAST_ELEMENT_TYPE"] = LAST_ELEMENT_TYPE;

   }

/********************************************************************************
  *
  * The function 
  *     NameEnforcer::def_reg(name_types)
  * is a helper function which checks to see if the current style file defined a
  * regular expression for the associated key name provided as an arguemnt.
  ******************************************************************************/
bool NameEnforcer::def_reg( name_types enum_elem  )
   {
     bool exists = false;
     if( s_definedRegularExpressions.find(enum_elem) != s_definedRegularExpressions.end()  )
	  exists = true;

     return exists;


   }



/********************************************************************************
  *
  * The function 
  *     NameEnforcer::get_reg(name_types)
  * is a helper function which checks to see if the current style file defined a
  * regular expression for the associated key name provided as an arguemnt. If so
  * it will return that regular expression as a string and if not it will end in
  * error.
  ******************************************************************************/
std::string NameEnforcer::get_reg( name_types enum_elem  )
   {
     std::map<name_types, std::string>::iterator mapElem = s_definedRegularExpressions.find(enum_elem);
  
     ROSE_ASSERT( mapElem != s_definedRegularExpressions.end());

     return mapElem->second;
   }


/***********************************************************************************
  *
  * The function 
  *     removeElements(std::list<SgNode*>)
  * will remove elements from the std::list<SgNode*> which the user has asked us to
  * filter out in the style file. The kind of key names which can specify this is
  *  filter_namespace
  *  keep_only_namespace
  *  filter_path
  *  filter_path_reg
  *  keep_only_path
  *  keep_only_path_reg
  ***********************************************************************************/
	std::list<SgNode*> 
NameEnforcer::removeElements(std::list<SgNode*>& nodeList)
   {
     std::list<SgNode*> returnList;

     for(std::list<SgNode*>::iterator construct = nodeList.begin();
		     construct != nodeList.end(); ++construct ){

	  bool keep_construct = true;
	  bool filter_on_namespace = false;

#if 0
	  //Since the ROSE ast is normalized we should have this rule to
	  //filter out any declaration statement which is not the first 
	  //non defining. This does not work completely yet on my version
	  //of ROSE
	  if( isSgDeclarationStatement(*construct) != NULL ){
	       if( isSgDeclarationStatement(*construct)->get_firstNondefiningDeclaration()
			       != (*construct) )
		    keep_construct = false;

	  }
#endif

       //Filer out constructs in namespace

	  std::list<std::string> scopes = findScopes(*construct);
	  name_types types = filter_namespace;

	  for ( std::multimap<name_types, std::string>::iterator  pos = filterPathMap.lower_bound(types);
			  pos != filterPathMap.upper_bound(types); ++pos) {
               if(  std::find( scopes.begin(), scopes.end(), pos->second) !=  scopes.end())
		  {

		    filter_on_namespace = true;
		    //std::cout << "    " << pos->second << std::endl;
		  }
	  }

	  types = keep_only_namespace;
	  for ( std::multimap<name_types, std::string>::iterator  pos = filterPathMap.lower_bound(types);
			  pos != filterPathMap.upper_bound(types); ++pos) {
               if(  std::find( scopes.begin(), scopes.end(), pos->second) ==  scopes.end())
		  {
		    keep_construct = false;

		  }
	  }

	  if( isSgLocatedNode(*construct) != NULL){
	       SgLocatedNode* locNode = isSgLocatedNode(*construct);
	       std::string filename = locNode->get_file_info()->get_filenameString();

	       if( locNode->get_file_info()->isCompilerGenerated() == true ){
		    keep_construct = false;
	       }else{

		    types = filter_path;

		    for ( std::multimap<name_types, std::string>::iterator  pos = filterPathMap.lower_bound(types);
				    pos != filterPathMap.upper_bound(types); ++pos) {

			 if( filename.find(pos->second) != std::string::npos )
			      keep_construct = false;
		    }

		    types = filter_path_reg;

		    for ( std::multimap<name_types, std::string>::iterator  pos = filterPathMap.lower_bound(types);
				    pos != filterPathMap.upper_bound(types); ++pos) {

			 boost::regex re;
			 boost::smatch what;

			 re.assign(pos->second, boost::regex_constants::perl);

			 if(  boost::regex_match(filename, what, re) == true ){
			      keep_construct=false;

			 }

		    }

		    types = keep_only_path;
		    for ( std::multimap<name_types, std::string>::iterator  pos = filterPathMap.lower_bound(types);
				    pos != filterPathMap.upper_bound(types); ++pos) {

			 if(filename.find(pos->second) == std::string::npos)
			      keep_construct = false;
		    }

		    types = keep_only_path_reg;
		    for ( std::multimap<name_types, std::string>::iterator  pos = filterPathMap.lower_bound(types);
				    pos != filterPathMap.upper_bound(types); ++pos) {

			 boost::regex re;
			 boost::smatch what;

			 re.assign(pos->second, boost::regex_constants::perl);

			 if(  boost::regex_match(filename, what, re) != true ){
			      keep_construct=false;

			 }
		    }
	       }

	  }

	  if( (filter_on_namespace == false) && (keep_construct == true) )
	       returnList.push_back(*construct);


     }
     return returnList;
   }

/********************************************************************************
  * The function
  * NameEnforcer::defaultVariableDeclaration(...)
  * is a helper function for the checkVariableDeclaration(..) function in the case where
  * the rules of the global scope or class fields did not apply.
  *****************************************************************************/
std::pair<std::string,name_types> 
NameEnforcer::defaultVariableDeclaration(SgType* varType, bool isStaticVariable){

     std::string error_message;
     name_types  unmatched_name_type = LAST_ELEMENT_TYPE; 
  //Handle if a variable is in global scope. If the regular expressions are not defined
  //default behavior if they are defined
     if( 
		     (isSgPointerType(varType) != NULL) &&
		     (def_reg(ptr_variable) == true) 
       )
	{

	  if(  isStaticVariable &&
			  (def_reg(static_ptr_variable) == true)){
	       error_message = " a static pointer variable ";
	       unmatched_name_type = static_ptr_variable;
	  }else{
	       error_message = " a pointer variable ";
	       unmatched_name_type = ptr_variable;
	  }

	}else if(   
			(isSgReferenceType(varType) != NULL ) &&
			( (def_reg(ref_global_variable) == true) |
			  (def_reg(ref_variable) == true)
			)
		) {

	     if(  isStaticVariable &&
			     (def_reg(static_ref_variable) == true)){
		  error_message = " a static reference variable ";
		  unmatched_name_type = static_ref_variable;
	     }else{
		  error_message = " a reference variable ";
		  unmatched_name_type = ref_variable;
	     }

	}else if(  isStaticVariable &&
			(def_reg(static_variable) == true)){
	     error_message = " a static variable ";
	     unmatched_name_type = static_variable;
	}else if( def_reg(variable) == true ){
	     error_message = " a variable ";
	     unmatched_name_type = variable;
	}

     return std::pair<std::string,name_types>(error_message, unmatched_name_type);
}

/*************************************************************************************
  *
  * The function
  *        NameEnforcer::checkVariableDeclaration()
  * will apply the rules on variable declarations.
  ************************************************************************************/
void 
NameEnforcer::checkVariableDeclaration(SgVariableDeclaration* variableDeclaration,
std::list< std::pair<name_types,SgNode*> >& violations

){

  ROSE_ASSERT(variableDeclaration != NULL);

  SgScopeStatement* scope = variableDeclaration->get_scope();

  SgInitializedNamePtrList variables = variableDeclaration->get_variables();
  bool isStaticVariable = variableDeclaration->get_declarationModifier().get_storageModifier().isStatic();
  for(SgInitializedNamePtrList::iterator name_it = variables.begin(); name_it != variables.end();
		  ++name_it)
     {
       SgInitializedName* initName = isSgInitializedName(*name_it);

       SgType*           varType = isSgInitializedName(initName)->get_type();
       if( isSgTypedefType(varType) != NULL )
	    varType = isSgTypedefType(varType)->get_base_type();

       std::string variable_name = initName->get_name().str();
       std::string error_message;
       std::string regular_expression;
       name_types unmatched_name_type = LAST_ELEMENT_TYPE;

       //Handle vaariable declarations of bool type specifically
       if( (isSgTypeBool(varType) != NULL) && ( def_reg(bool_type_variable) == true ) ){
	    boost::regex re;
	    boost::smatch what;

            regular_expression = s_definedRegularExpressions.find(bool_type_variable)->second;
	    re.assign(regular_expression, boost::regex_constants::perl);

	    if(  boost::regex_match(variable_name, what, re) == false ){
		 error_message = " variable declaration of bool type ";
		 std::cout << "Warning:" << error_message << "\"" << variable_name 
			 << "\" did not match regexp \"" << regular_expression << "\"" << std::endl;
		 Sg_File_Info* fileInfo = initName->get_file_info();
		 std::cout << "        file: " << fileInfo->get_filenameString() << " line " 
			 << fileInfo->get_line() << " col " << fileInfo->get_col() << std::endl;
		 violations.push_back(std::pair<name_types,SgInitializedName*>(bool_type_variable,initName) );
		
		 //findUses(variableDeclaration);
		 error_message = "";

	    }
    

        }

	       
   //Handle variables on the stack. 
       if ( (isSgClassDefinition(scope) == NULL) &&
		       (isSgNamespaceDefinitionStatement(scope) == NULL) &&
		       (isSgGlobal(scope) == NULL)
	  )
	  {
	    //nstd::cout << "Looking on stack" << std::endl;
	    if( (isSgPointerType(varType) != NULL) &&
			    ( (def_reg(ptr_stack_variable) == true) |
			      (def_reg(static_ptr_global_variable) == true) 
			    )
	      ){
		 if( isStaticVariable &&
				 (def_reg(static_ptr_stack_variable) == true)){
		      error_message = " a static stack pointer variable ";
		      unmatched_name_type = static_ptr_stack_variable;
		 }else if(def_reg(ptr_stack_variable) == true){
		      error_message = " a stack pointer variable ";
		      unmatched_name_type = ptr_stack_variable;
		 }
	    }else if( (isSgReferenceType(varType) != NULL ) &&
			    ( (def_reg(ref_stack_variable) == true) |
			      (def_reg(static_ref_stack_variable) == true)
			    )
		    ) {
		 if( isStaticVariable &&
				 (def_reg(static_ref_stack_variable) == true)){
		      error_message = " a static stack reference variable ";
		      unmatched_name_type = static_ref_stack_variable;
		 }else if(def_reg(ref_stack_variable) == true){
		      error_message = " a stack reference variable ";
		      unmatched_name_type = ref_stack_variable;
		 }
	    }else if( isStaticVariable &&
			    (def_reg(static_stack_variable) == true)){
		 error_message = " a static stack variable ";
		 unmatched_name_type = static_stack_variable;
	    }else if( def_reg(stack_variable) == true ){
		 error_message = " a stack variable ";
		 unmatched_name_type = stack_variable;
	    }
	  }


    //Handle if a variable is in global scope. If the regular expressions are not defined
    //default behavior if they are defined
       if (isSgGlobal(scope) != NULL)
	  {
	    if( (isSgPointerType(varType) != NULL) &&
			    ( (def_reg(ptr_global_variable) == true) |
			      (def_reg(static_ptr_global_variable) == true) 
			    )
	      ){
		 if( isStaticVariable &&
				 (def_reg(static_ptr_global_variable) == true)){
		      error_message = " a static global pointer variable ";
		      unmatched_name_type = static_ptr_global_variable;
		 }else if(def_reg(ptr_global_variable) == true){
		      error_message = " a global pointer variable ";
		      unmatched_name_type = ptr_global_variable;
		 }
	    }else if( (isSgReferenceType(varType) != NULL ) &&
			    ( (def_reg(ref_global_variable) == true) |
			      (def_reg(static_ref_global_variable) == true)
			    )
		    ) {
		 if( isStaticVariable &&
				 (def_reg(static_ref_global_variable) == true)){
		      error_message = " a static global reference variable ";
		      unmatched_name_type = static_ref_global_variable;
		 }else if(def_reg(ref_global_variable) == true){
		      error_message = " a global reference variable ";
		      unmatched_name_type = ref_global_variable;
		 }
	    }else if( isStaticVariable &&
			    (def_reg(static_global_variable) == true)){
		 error_message = " a static global variable ";
		 unmatched_name_type = static_global_variable;
	    }else if( def_reg(global_variable) == true ){
		 error_message = " a global variable ";
		 unmatched_name_type = global_variable;
	    }
	  }
    //Handle if a variable a class fieldd. If the regular expressions are not defined
    //default behavior if they are defined

       if (isSgClassDefinition(scope) != NULL)
	  {
	    //std::cout << "class def" << std::endl;
	    if( (isSgPointerType(varType) != NULL) &&
			    ( (def_reg(ptr_class_field) == true) |
			      ( def_reg(static_ptr_class_field)==true ) 
			    )
	      ){
		 if( isStaticVariable &&
				 (def_reg(static_ptr_class_field) == true)){
		      error_message = " a static class field pointer variable ";
		      unmatched_name_type = static_ptr_class_field;
		 }else if(def_reg(ptr_class_field) == true){
		      error_message = " a pointer class field ";
		      unmatched_name_type = ptr_class_field;
		 }
	    }else if( (isSgReferenceType(varType) != NULL ) &&
			    ( (def_reg(ref_class_field) == true) |
			      (def_reg(static_ref_class_field) == true)
			    )
		    ) {
		 //std::cout << "a reference " << std::endl;
		 if( isStaticVariable &&
				 (def_reg(static_ref_class_field) == true)){
		      //std::cout << " a static class field reference" << std::endl;
		      error_message = " a static class field reference variable ";
		      unmatched_name_type = static_ref_class_field;
		 }else  if(def_reg(ref_class_field) == true){
		      error_message = " a reference class field ";
		      unmatched_name_type = ref_class_field;
		 }
	    }else if( isStaticVariable &&
			    (def_reg(static_class_field) == true)){
		 error_message = " a static class field variable ";
		 unmatched_name_type = static_class_field;
	    }else if( def_reg(class_field) == true ){
		 error_message = " a class field ";
		 unmatched_name_type = class_field;
		    }
	  }


       if(unmatched_name_type == LAST_ELEMENT_TYPE){
	    //std::cout << "Went into default" << std::endl;
	    std::pair<std::string, name_types> ret = defaultVariableDeclaration(varType,isStaticVariable);
	    error_message = ret.first;
	    unmatched_name_type = ret.second;
       }
       //if(unmatched_name_type == LAST_ELEMENT_TYPE)
	//    std::cout << "THAT WAS A LAST ELEMENT" << std::endl;
       if( s_definedRegularExpressions.find(unmatched_name_type) != s_definedRegularExpressions.end()  )
	  {
	    regular_expression = s_definedRegularExpressions.find(unmatched_name_type)->second;
	  }else
	  {
	   // std::cout << "Did not find " << unmatched_name_type << std::endl;
	  }
       //std::cout << "Checking: " << regular_expression << " " << variable_name << std::endl;

       if((unmatched_name_type != LAST_ELEMENT_TYPE ) && ( variable_name!="") && (def_reg(unmatched_name_type) == true) ){
	    boost::regex re;
	    boost::smatch what;
	    re.assign(regular_expression, boost::regex_constants::perl);

	    if(  boost::regex_match(variable_name, what, re) == false ){
		 std::cout << "Warning:" << error_message << "\"" << variable_name 
			 << "\" did not match regexp \"" << regular_expression << "\"" << std::endl;
		 Sg_File_Info* fileInfo = initName->get_file_info();
		 std::cout << "        file: " << fileInfo->get_filenameString() << " line " 
			 << fileInfo->get_line() << " col " << fileInfo->get_col() << std::endl;
		 violations.push_back(std::pair<name_types,SgInitializedName*>(unmatched_name_type,initName) );
		 //findUses(variableDeclaration);
	    }
       }

     }//End of iteration over names

}

/*************************************************************************************
 *
 * The function
 *        NameEnforcer::checkFunctionDeclaration()
 * will apply the rules on function declarations.
 ************************************************************************************/
void
NameEnforcer::checkFunctionDeclaration(SgFunctionDeclaration* functionDeclaration,
   std::list< std::pair<name_types,SgNode*> >& violations
){

	  ROSE_ASSERT(functionDeclaration != NULL);
	  std::string functionName = functionDeclaration->get_name().str();

	  std::string error_message;
	  name_types unmatched_name_type = LAST_ELEMENT_TYPE;
	  bool isStaticFunction = functionDeclaration->get_declarationModifier().get_storageModifier().isStatic();
	  if( isSgMemberFunctionDeclaration(functionDeclaration) != NULL ){
	       if( isStaticFunction &&
			       (def_reg(static_member_function) == true)){
		    error_message = " a static member function ";
		    unmatched_name_type = static_member_function;
	       }else if(def_reg(member_function) == true){
		    error_message = " a member function ";
		    unmatched_name_type = member_function;
	       }

	  }else {
	       if( isStaticFunction &&
			       (def_reg(static_global_function) == true)){
		    error_message = " a static global function ";
		    unmatched_name_type = static_global_function;
	       }else if(def_reg(global_function) == true){
		    error_message = " a global function ";
		    unmatched_name_type = global_function;
	       }
	  }

	  if( unmatched_name_type == LAST_ELEMENT_TYPE ){
	       if( isStaticFunction &&
			       (def_reg(static_function) == true)){
		    error_message = " a static function ";
		    unmatched_name_type = static_function;
	       }else if(def_reg(function) == true){
		    error_message = " a function ";
		    unmatched_name_type = function;
	       }
	  }

	  if((unmatched_name_type != LAST_ELEMENT_TYPE ) && ( functionName!="") && (def_reg(unmatched_name_type) == true) ){
	       boost::regex re;
	       boost::smatch what;
	       std::string regular_expression = s_definedRegularExpressions[unmatched_name_type];

	       re.assign(regular_expression, boost::regex_constants::perl);

	       if(  boost::regex_match(functionName, what, re) == false ){
		    std::cout << "Warning:" << error_message << "\"" << functionName
			    << "\" did not match regexp \"" << regular_expression << "\"" << std::endl;
		    Sg_File_Info* fileInfo = functionDeclaration->get_file_info();
		    std::cout << "        file: " << fileInfo->get_filenameString() << " line " 
			    << fileInfo->get_line() << " col " << fileInfo->get_col() << std::endl;
		    violations.push_back(std::pair<name_types,SgNode*>(unmatched_name_type,functionDeclaration) );

		    //findUses(functionDeclaration);
	       }
	  }


}

/*************************************************************************************
 *
 * The function
 *        NameEnforcer::checkClassDeclaration()
 * will apply the rules on class declarations.
 ************************************************************************************/
void 
NameEnforcer::checkClassDeclaration(SgClassDeclaration* classDeclaration,
std::list< std::pair<name_types,SgNode*> >& violations
){
  ROSE_ASSERT(classDeclaration != NULL);


  std::string className = classDeclaration->get_name().str();

  std::string error_message;

  if( ( className!="") && (def_reg(class_name) == true) ){
       boost::regex re;
       boost::smatch what;
       std::string regular_expression = s_definedRegularExpressions[class_name];

       re.assign(regular_expression, boost::regex_constants::perl);

       if(  boost::regex_match(className, what, re) == false ){
	    std::cout << "Warning:" << error_message << "\"" << className
		    << "\" did not match regexp \"" << regular_expression << "\"" << std::endl;
	    Sg_File_Info* fileInfo = classDeclaration->get_file_info();
	    std::cout << "        file: " << fileInfo->get_filenameString() << " line " 
		    << fileInfo->get_line() << " col " << fileInfo->get_col() << std::endl;
	    violations.push_back(std::pair<name_types,SgNode*>(class_name,classDeclaration) );

	    //findUses(classDeclaration);
       }
  }

}



/****************************************************
  * BEGIN visitorTraversal
  * class visitorTraversal
  * is used in order to support extraction the PreprocessingInfo annotation
  * from the AST. 
  *************************************************************************/
void
SynthesizedAttribute::display() const
   {
     std::list<PreprocessingInfo*>::const_iterator i = accumulatedList.begin();
     while (i != accumulatedList.end())
	{
	  printf ("CPP define directive = %s \n",(*i)->getString().c_str());
	  i++;
	}
   }

class visitorTraversal : public AstBottomUpProcessing<SynthesizedAttribute>
   {
     public:
       // virtual function must be defined
	  virtual SynthesizedAttribute evaluateSynthesizedAttribute ( 
			  SgNode* n, SynthesizedAttributesList childAttributes );
   };

	SynthesizedAttribute
visitorTraversal::evaluateSynthesizedAttribute ( SgNode* n, SynthesizedAttributesList childAttributes )
   {
     SynthesizedAttribute localResult;

  // printf ("In evaluateSynthesizedAttribute(n = %p = %s) \n",n,n->class_name().c_str());

  // Build the list from children (in reverse order to preserve the final ordering)
     for (SynthesizedAttributesList::reverse_iterator child = childAttributes.rbegin(); child != childAttributes.rend(); child++)
	{
	  localResult.accumulatedList.splice(localResult.accumulatedList.begin(),child->accumulatedList);
	}

  // Add in the information from the current node
     SgLocatedNode* locatedNode = isSgLocatedNode(n);
     if (locatedNode != NULL)
	{
	  AttachedPreprocessingInfoType* commentsAndDirectives = locatedNode->getAttachedPreprocessingInfo();

	  if (commentsAndDirectives != NULL)
	     {
	    // printf ("Found attached comments (to IR node at %p of type: %s): \n",locatedNode,locatedNode->class_name().c_str());
	    // int counter = 0;

	    // Use a reverse iterator so that we preserve the order when using push_front to add each directive to the accumulatedList
	       AttachedPreprocessingInfoType::reverse_iterator i;
	       for (i = commentsAndDirectives->rbegin(); i != commentsAndDirectives->rend(); i++)
		  {
		 // The different classifications of comments and directives are in ROSE/src/frontend/SageIII/rose_attributes_list.h
		    if ((*i)->getTypeOfDirective() == PreprocessingInfo::CpreprocessorDefineDeclaration)
		       {
		      // use push_front() to end up with source ordering of final list of directives
			 localResult.accumulatedList.push_front(*i);
		       }
		  }
	     }
	}

  // printf ("localResult after adding current node info \n");
  // localResult.display();

     return localResult;
   }


/******************************************************************
  * END visitorTraversal
  ****************************************************************/

/*************************************************************************************
 *
 * The function
 *        NameEnforcer::checkMacroNames()
 * will apply the rules on macro names.
 ************************************************************************************/

void
NameEnforcer::checkMacroNames(PreprocessingInfo* currentInfo,
std::list< std::pair<name_types,PreprocessingInfo*> >& violations
)
   {

     ROSE_ASSERT(currentInfo != NULL);
//#ifdef USE_ROSE_BOOST_WAVE_SUPPORT 
    	  PreprocessingInfo::r_macro_def* macro_def = currentInfo->get_macro_def();

	  std::string macroName = macro_def->macro_name.get_value().c_str();

	  if( def_reg(macro_name) == true ){
	       boost::regex re;
	       boost::smatch what;
	       std::string regular_expression = s_definedRegularExpressions[macro_name];

	       re.assign(regular_expression, boost::regex_constants::perl);

	       if(  boost::regex_match(macroName, what, re) == false ){
		    std::cout << "Warning: a macro def \"" << macroName
			    << "\" did not match regexp \"" << regular_expression << "\"" << std::endl;
		    Sg_File_Info* fileInfo = currentInfo->get_file_info();
		    std::cout << "        file: " << fileInfo->get_filenameString() << " line " 
			    << fileInfo->get_line() << " col " << fileInfo->get_col() << std::endl;
		    violations.push_back(std::pair<name_types,PreprocessingInfo*>(macro_name,currentInfo));

	       }
	  }
//#endif
   }


/*************************************************************************************
 *
 * The function
 *        NameEnforcer::checkEnumDeclaration()
 * will apply the rules on enum declarations names.
 ************************************************************************************/
void
NameEnforcer::checkEnumDeclaration(SgEnumDeclaration* enumDeclaration, std::list< std::pair<name_types,SgNode*> >& 
violations){
     ROSE_ASSERT(enumDeclaration != NULL);
	  std::string enumName = enumDeclaration->get_name().str();

	  std::string error_message = " an enum declaration ";

	  if( ( enumName!="") && (def_reg(enum_name) == true) ){
	       boost::regex re;
	       boost::smatch what;
	       std::string regular_expression = s_definedRegularExpressions[enum_name];

	       re.assign(regular_expression, boost::regex_constants::perl);

	       if(  boost::regex_match(enumName, what, re) == false ){
		    std::cout << "Warning:" << error_message << "\"" << enumName
			    << "\" did not match regexp \"" << regular_expression << "\"" << std::endl;
		    Sg_File_Info* fileInfo = enumDeclaration->get_file_info();
		    std::cout << "        file: " << fileInfo->get_filenameString() << " line " 
			    << fileInfo->get_line() << " col " << fileInfo->get_col() << std::endl;
		    violations.push_back(std::pair<name_types,SgNode*>(enum_name,enumDeclaration) );

	       }
	  }

}


/*************************************************************************************
 *
 * The function
 *        NameEnforcer::checkEnumLabels()
 * will apply the rules on enum labels.
 ************************************************************************************/
void 
NameEnforcer::checkEnumLabels(SgEnumDeclaration* enumDeclaration,
std::list< std::pair<name_types,SgNode*> >& violations
){
     ROSE_ASSERT(enumDeclaration != NULL);

	  std::string enumName = enumDeclaration->get_name().str();

	  std::string error_message = " an enum declaration ";


	  SgInitializedNamePtrList & enumLabels = enumDeclaration->get_enumerators();


	  SgInitializedNamePtrList::iterator itLabel = enumLabels.begin();



	  
	  for(; itLabel != enumLabels.end(); ++itLabel)
	     {
	       std::string enumLabel = (*itLabel)->get_name().str();
	       SgInitializedName* initName = (*itLabel);
	       if( ( enumName!="") && (def_reg(enum_label) == true) ){
		    boost::regex re;
		    boost::smatch what;
		    std::string regular_expression = s_definedRegularExpressions[enum_label];

		    re.assign(regular_expression, boost::regex_constants::perl);
		    if(  boost::regex_match(enumLabel, what, re) == false ){
			 std::cout << "Warning:" << error_message << "\"" << enumLabel
				 << "\" did not match regexp \"" << regular_expression << "\"" << std::endl;
			 Sg_File_Info* fileInfo = initName->get_file_info();
			 std::cout << "        file: " << fileInfo->get_filenameString() << " line " 
				 << fileInfo->get_line() << " col " << fileInfo->get_col() << std::endl;
			 violations.push_back(std::pair<name_types,SgInitializedName*>(enum_label,initName) );

		    }
	       }

	     }
}

template<typename CurrentType>
std::vector<SgNode*> findingNodes2(SgNode* curr_node_before,SgNode* locNode){


     CurrentType curr_node = dynamic_cast<CurrentType>(curr_node_before);
     ROSE_ASSERT(curr_node != NULL);
     std::vector<SgNode*> returnType;


     bool addNode = false;

     if(isSgLocatedNode(curr_node) != NULL)
     if( (curr_node != locNode) /*&& (curr_node->get_file_info()->isCompilerGenerated() == false)*/ )  {
	  std::vector< std::pair<SgNode *, std::string > >
		  dataMemberPointer = curr_node->returnDataMemberPointers();
	  for( std::vector< std::pair<SgNode *, std::string > >::iterator dataMemb_it =
			  dataMemberPointer.begin();
			  dataMemb_it != dataMemberPointer.end(); ++dataMemb_it){

	       SgNode* dataMemb = (*dataMemb_it).first;

	       if(  (dataMemb == locNode)  /*&& (dataMemb->get_file_info()->isCompilerGenerated() == false)*/ 
			       && ( (*dataMemb_it).second != std::string("parent") )
			       && ( (*dataMemb_it).second != std::string("definingDeclaration")) 
			       && ( (*dataMemb_it).second != std::string("firstNondefiningDeclaration"))       
				  

		 ){
		    addNode = true;
		    std::cout << "The token is:" << (*dataMemb_it).second << std::endl;
		    break;
	       }
	  }
     }


	  if(addNode == true){
		    Sg_File_Info* fileInfo = curr_node->get_file_info();
		    std::cout << "Use in file: " << fileInfo->get_filenameString() << " line " << fileInfo->get_file_info()->get_filenameString()
			    << fileInfo->get_line() << " col " << fileInfo->get_col() << std::endl;

		    returnType.push_back( curr_node );

	  }

     return returnType;


}


std::vector< std::pair<SgNode*,std::string> > 
NameEnforcer::findUses(SgLocatedNode* locNode){
     std::vector<std::pair<SgNode*,std::string> > returnType;



     NodeQuery::queryMemoryPool(
		     std::bind2nd(std::ptr_fun(findingNodes2<SgNode*>), locNode)
		     );




     return returnType;

};


/***************************************************
 * INITIATE checking of all rules specified in the styles
 *****************************************************/
void NameEnforcer::enforceRules(SgNode* node, std::list< std::pair<name_types,SgNode*> >& violations,
                               std::list< std::pair<name_types,PreprocessingInfo*> >& macroViolations ){
     if( SgClassDeclaration* classDecl = isSgClassDeclaration(node) )
          checkClassDeclaration(classDecl, violations);

     if( SgVariableDeclaration* varDecl = isSgVariableDeclaration(node) )
          checkVariableDeclaration(varDecl, violations);
     if( SgFunctionDeclaration* funcDecl = isSgFunctionDeclaration(node) )
          checkFunctionDeclaration( funcDecl, violations );
    
     //checkMacroNames(project);
     if( SgEnumDeclaration* enumDecl = isSgEnumDeclaration(node) ){
         checkEnumDeclaration( enumDecl, violations );
         checkEnumLabels( enumDecl,violations );
     }
  // Add in the information from the current node
     if (SgLocatedNode* locatedNode = isSgLocatedNode(node))
	{
	  AttachedPreprocessingInfoType* commentsAndDirectives = locatedNode->getAttachedPreprocessingInfo();

	  if (commentsAndDirectives != NULL)
	     {
	    // printf ("Found attached comments (to IR node at %p of type: %s): \n",locatedNode,locatedNode->class_name().c_str());
	    // int counter = 0;

	    // Use a reverse iterator so that we preserve the order when using push_front to add each directive to the accumulatedList
	       AttachedPreprocessingInfoType::reverse_iterator i;
	       for (i = commentsAndDirectives->rbegin(); i != commentsAndDirectives->rend(); i++)
		  {
		 // The different classifications of comments and directives are in ROSE/src/frontend/SageIII/rose_attributes_list.h
		    if ((*i)->getTypeOfDirective() == PreprocessingInfo::CpreprocessorDefineDeclaration)
		       {
                         checkMacroNames(*i, macroViolations);
		       }
		  }
	     }
	}

};

 }; //End namespace NameConsistency
}; //end namespace CompassAnalyses



