#include "rose.h"
#include <sageDoxygen.h>
#include <list>
#include <commandline_processing.h>
#include <string>
#include <limits.h>


//Header files to make a directory
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;



//Adds a new string to the description of a doxygen comment
class AddStringToDescription : public AstSimpleProcessing 
   {

     public:

	  std::vector<DoxygenComment*> deprecatedComments;
          std::vector<DoxygenComment*> internalInterfaceComments;
          std::vector<DoxygenComment*> classMemberComments;
          std::vector<DoxygenComment*> userInterfaceComments;


          virtual void visit(SgNode *n) 
             {
               SgDeclarationStatement *ds = isSgDeclarationStatement(n);
               if (!Doxygen::isRecognizedDeclaration(ds)) return;
               if (ds) 
                  {
                    list<DoxygenComment *> *commentList = Doxygen::getCommentList(ds);
                    for (list<DoxygenComment *>::iterator i = commentList->begin(); i != commentList->end(); ++i) 
                       {
                         DoxygenComment *c = *i;
                         if (c->originalFile == NULL)
                            {
			      //std::cout << "OriginalFile is not null" << c->entry.unparse() << std::endl;
                              continue;
                            }


						 std::string prototype = c->entry.prototype();


						   
						 string::size_type location = prototype.find("::get_");
						 if( location == string::npos)
                             location = prototype.find("::set_");

						 if( location == string::npos )
						   continue;

						 //This is when the mangled name begins
						 int beginName =  prototype.find_last_of(" ");

						 int endName   =  prototype.find("(");
						 //std::cout << "Prototype is: " << prototype << std::endl;
                         if( beginName == string::npos )
						   beginName = 0;
						 ROSE_ASSERT(beginName != string::npos);
						 ROSE_ASSERT(endName != string::npos);


						 std::string varName = prototype.substr(beginName, location+2-beginName)
						                       + prototype.substr(location+6, endName-(location+6));

						 std::string message = "See " + varName + " for documentation " ;

						 //AS(1/25/08)
						 //The DoxygenEntry::Function doxygen syntax does not have a description construct.
						 //It is therefore necessary to use the brief construct instead for that class.
						 //FIXME: Should we use brief in all cases?
						 if(c->entry.type() == DoxygenEntry::Function)
						 {
                             if(c->entry.hasBrief() == false)
	    					 {
		    				   std::string* strPtr = &c->entry.brief();
			    			   strPtr = new string();
                               //c->entry.description() = strPtr;
				    		   c->entry.set_brief(message);
						      //std::cout << "Unparse2: " << c->entry.description() << std::endl;

						     }else{
						       c->entry.brief() +=message;
						       //std::cout << "Unparse: \"" << c->entry.unparse() << "\""<<std::endl;
						     }
						 }else{
     						 if(c->entry.hasDescription() == false)
	    					 {

		    				   std::string* strPtr = &c->entry.description();
			    			   strPtr = new string();
                               //c->entry.description() = strPtr;
				    		   c->entry.set_description(message);
						      //std::cout << "Unparse2: " << c->entry.description() << std::endl;

						     }else{
						       c->entry.description() +=message;
						       //std::cout << "Unparse: \"" << c->entry.unparse() << "\""<<std::endl;
						     }

						 }
						 c->originalComment->setString(c->entry.unparse());


                       }
                  }
             }
   };




//Orginizes all comments that are not already in a group into groups
class OrganizeAllComments : public AstSimpleProcessing 
{

  public:

	//These variables are needed because the groups are created when
	//the first variable is put into them. The order of the groups 
	//are dependent on when they were created relative to each other
	//and that is what we want to control.
	std::vector<DoxygenComment*> deprecatedComments;
	std::vector<DoxygenComment*> internalInterfaceComments;
	std::vector<DoxygenComment*> classMemberComments;
	std::vector<DoxygenComment*> userInterfaceComments;


	virtual void visit(SgNode *n) 
	{
	  SgDeclarationStatement *ds = isSgDeclarationStatement(n);
	  if (!Doxygen::isRecognizedDeclaration(ds)) return;
	  if (ds) 
	  {

		//For each comment determine which group it should go into.
		//It will be inserted into a group by the attachComments(..) member
		//function.
		list<DoxygenComment *> *commentList = Doxygen::getCommentList(ds);
		for (list<DoxygenComment *>::iterator i = commentList->begin(); i != commentList->end(); ++i) 
		{
		  DoxygenComment *c = *i;
		  if (c->originalFile == NULL)
		  {
			//std::cout << "OriginalFile is not null" << c->entry.unparse() << std::endl;
			continue;
		  }

		  //Do not group classes
		  if (c->entry.type() == DoxygenEntry::Class )
			continue;

		  //Determine which group the comments should go into
		  if (c->entry.hasDeprecated())
		  {
			//c->attach(c->originalFile, c->originalFile->group("Deprecated"));
			deprecatedComments.push_back(c);
		  }
		  else if(isSgVariableDeclaration(n) != NULL ){
			classMemberComments.push_back(c);
			//c->attach(c->originalFile, c->originalFile->group("Class Members"));

		  }else if( (c->entry.hasBrief()) &&
			  (c->entry.brief().find("\\b FOR \\b INTERNAL \\b USE") != std::string::npos ) ){
			internalInterfaceComments.push_back(c);
			//c->attach(c->originalFile, c->originalFile->group("FOR INTERNAL USE"));

		  }else
		  {
			userInterfaceComments.push_back(c);
			//c->attach(c->originalFile, c->originalFile->group("User Interface"));
		  }
		}
	  }
	}

	//The reason why the comments are put into groups in a separate function is that the
	//order in which the groups appear is determined by when the groups are created.
	//The groups are created when comments are first put into them. If the groups already
	//exist they can currently not be sorted.
	void attachComments(){

	  //Each of these for loops will put the comments into a group. The group names are 
	  //determined by the 'c->originalFile->group("Deprecated")' command where "Deprecated"
	  //is the group name.
	  
	  //	       std::cout << "deprecatedComments: " << deprecatedComments.size() << std::endl;
	  for(std::vector<DoxygenComment*>::iterator iItr = deprecatedComments.begin();
		  iItr != deprecatedComments.end(); ++iItr){
		DoxygenComment* c = *iItr;
		c->attach(c->originalFile, c->originalFile->group("Deprecated"));


	  }

	  //	       std::cout << "classMemberComments: " << classMemberComments.size() << std::endl;

	  for(std::vector<DoxygenComment*>::iterator iItr = classMemberComments.begin();
		  iItr != classMemberComments.end(); ++iItr){
		DoxygenComment* c = *iItr;
		c->attach(c->originalFile, c->originalFile->group("Class Members"));

	  }

	  //	       std::cout << "internalInterfaceComments: " << internalInterfaceComments.size() << std::endl;
	  for(std::vector<DoxygenComment*>::iterator iItr = internalInterfaceComments.begin();
		  iItr != internalInterfaceComments.end(); ++iItr){
		DoxygenComment* c = *iItr;
		c->attach(c->originalFile, c->originalFile->group("Internal Interface"));
	  }

	  //	       std::cout << "userInterfaceComments: " << userInterfaceComments.size() << std::endl;

	  for(std::vector<DoxygenComment*>::iterator iItr = userInterfaceComments.begin();
		  iItr != userInterfaceComments.end(); ++iItr){
		DoxygenComment* c = *iItr;
		c->attach(c->originalFile, c->originalFile->group("User Interface"));

	  }


	};
};



//Returns true if substring is a substring of the string s
bool str_contains_substr(std::string& s, std::string& substring)
      {

        if( (s.length() >= substring.length()) && (s.substr(0,substring.length()) == substring) )
            {
	     //std::cout << "s.substr(0,substring.length() " << s.substr(0,substring.length()) <<
	     //             " == " << substring << std::endl;
	     return true;
	}else{
             //std::cout << "s.substr(0,substring.length() " << s.substr(0,substring.length()) <<
             //		     " != " << substring << std::endl;
             return false;
        } 
      };


//Return the index of the first string in the std::vector<std::string> that is a
//substring of the std::string. If not such substring is found return INT_MAX.
int priority_of_substr_find(std::string s, std::vector<std::string> identifierPrefix)
     {

        //std::cout << "priority_of_substr \n\n SAFSDFAS" << std::endl;
        for(unsigned int i = 0; i < identifierPrefix.size(); ++i)
	   {
            if( s.find(identifierPrefix[i]) != string::npos  )
	        return i;
	   }

        return INT_MAX;
    };




//Sort all DoxygenComment* that is not part of the header denoting the beginning of
//a group (//!{) according to a predefined criteria.
bool sort_comments(DoxygenComment* smaller, DoxygenComment* bigger ) 
	     { 
	       bool returnValue = false;
               

	       if( smaller == NULL || bigger == NULL )
		  {
                     return returnValue;   
		  }

	       //Criteria: all comments are sorted with comments containing "get_" before
	       //"set_" comments and all other functions are alphabetically sorted after that.
	       if( (smaller->entry.type() == DoxygenEntry::Function ) 
			       && ( bigger->entry.type() == DoxygenEntry::Function )  )
		  {

		    std::cout << "Sorting functions" << std::endl << std::endl;

		    std::vector<std::string> identifierPrefix;
 
		    identifierPrefix.push_back("get_");
                    identifierPrefix.push_back("set_");


		    //std::cout << "Name: " << smaller->entry.name() << std::endl;
		    //std::cout << "Unparse:: " << smaller->entry.unparse() << std::endl;
		    if(smaller->entry.hasPrototype() && bigger->entry.hasPrototype())
		    //std::cout << "Prototype: " << smaller->entry.prototype() << std::endl;

                     if( priority_of_substr_find(smaller->entry.prototype(), identifierPrefix) == 
				                         priority_of_substr_find(bigger->entry.prototype(),  identifierPrefix)   )
			{

			  std::string smallerValue = smaller->entry.prototype();
			  std::string biggerValue  = bigger->entry.prototype();

			  //Do a lexical sort on function names
			  if( smallerValue.size() < biggerValue.size() )
                               returnValue  =  std::lexicographical_compare(smallerValue.begin(), smallerValue.begin()+
						      smallerValue.size(), biggerValue.begin() , biggerValue.begin()+smallerValue.size() );
			  else
                               returnValue  =  std::lexicographical_compare(smallerValue.begin(), smallerValue.begin()+
						      biggerValue.size(), biggerValue.begin() , biggerValue.begin()+biggerValue.size() );


			  //std::cout << smallerValue << " comparesLike " << returnValue << "  " << biggerValue << std::endl;

			}

		     else if( priority_of_substr_find(smaller->entry.prototype(), identifierPrefix) > 
		        priority_of_substr_find(bigger->entry.prototype(),  identifierPrefix)   )
			 returnValue = true;
		  }else if(smaller->entry.type() == DoxygenEntry::Variable    ){
		       //Criteria: All variables are sorted lexically
	          if(smaller->entry.hasPrototype() && bigger->entry.hasPrototype()){
	   
			  std::string smallerValue = smaller->entry.prototype();
			  std::string biggerValue  = bigger->entry.prototype();

			  //Do a lexical sort on variable names
			  if( smallerValue.size() < biggerValue.size() )
                               returnValue  =  std::lexicographical_compare(smallerValue.begin(), smallerValue.begin()+
						      smallerValue.size(), biggerValue.begin() , biggerValue.begin()+smallerValue.size() );
			  else
                               returnValue  =  std::lexicographical_compare(smallerValue.begin(), smallerValue.begin()+
						      biggerValue.size(), biggerValue.begin() , biggerValue.begin()+biggerValue.size() );


			  }
			  //std::cout << smallerValue << " comparesLike " << returnValue << "  " << biggerValue << std::endl;
    
		       
		  };

	       return returnValue;
	     }


//Function to sort all DoxygenComment comments in all files
void
sortComments(SgProject *p, bool (*pred)(DoxygenComment*,DoxygenComment*) )
   {
     map<string, DoxygenFile *> *docsList = Doxygen::getFileList(p);
     for (map<string, DoxygenFile *>::iterator i = docsList->begin(); i != docsList->end(); ++i)
        {  
	  //std::cout << i->first << " " ; 
	   (*i).second->sortComments(pred);
        }

   }



//Criteria for filtering out documentation we are not interested in unparsing
bool criteriaForUnparsingNewDocumentation(DoxygenFile*, std::string& fileName  ) 
{
  bool shouldUnparse = true;

  if( (fileName.find("bar.docs") != string::npos) ||
      (fileName.find("__") != string::npos) ||
      (fileName.find("std::") != string::npos) ||
      (fileName.find("StorageClass") != string::npos)
	)
	 shouldUnparse = false;

  return shouldUnparse;
};


//Criteria for filtering out documentation we are not interested in unparsing
//when we are only interest in the Binary Documentation
bool unparseBinaryDocumentation(DoxygenFile*, std::string& fileName  ) 
{
  bool shouldUnparse = true;

  if( (fileName.find("bar.docs") != string::npos) ||
      (fileName.find("__") != string::npos) ||
      (fileName.find("std::") != string::npos) ||
      (fileName.find("StorageClass") != string::npos) ||
	  (fileName.find("SgBinary") == string::npos)
	  
	)
	 shouldUnparse = false;

//  std::cout << fileName << std::endl;
//   std::string path     = StringUtility::getPathFromFileName(fileName);
//   std::string fname = StringUtility::stripPathFromFileName(fileName); 
   fileName= "binary/"+fileName;
   /*
   if(path == "")
	 path = "./binary/";
   else 
	 path = path + "/binary/";
  fileName = path+fname;
//  mkdir(path.c_str(),777); 
  std::cout << "Unparse to " << fileName << std::endl;
*/
  return shouldUnparse;
};

//Criteria for filtering out documentation we are not interested in unparsing
bool criteriaForGeneratingOrUpdatingDocumenatation(SgNode* node) 
{
  bool shouldHaveDocs = true;

  SgLocatedNode* classDecl = isSgLocatedNode(node); 
  if( classDecl != NULL )
  {
//	if(classDecl->get_file_info()->get_filenameString().find("/home/saebjornsen1/") != string::npos)
	if(classDecl->get_file_info()->get_filenameString().find("/SageIII/") == string::npos)
	  shouldHaveDocs = false;

  }
  

  return shouldHaveDocs;
};


int main( int argc, char * argv[] ) 
   {
     //Parse commandline and makes sure all comments and directives in all
     //files is collected. This is important since we have separate doxygen
     //*.docs files

     vector<string> argvList(argv, argv + argc);

     CommandlineProcessing::addCppSourceFileSuffix(".docs");
     CommandlineProcessing::addCppSourceFileSuffix(".h");

     commentMap = std::map<const PreprocessingInfo*, DoxygenComment* >();

     Doxygen::parseCommandLine(argvList);

     vector<string> newArgv = argvList;
     newArgv.insert(newArgv.begin() + 1, "-rose:collectAllCommentsAndDirectives");

  // DQ (5/21/2010): Added macro to turn off the compilation of Wave when 
  // compiling ROSE source codes (only effects required header files).
     newArgv.insert(newArgv.begin() + 1, "-DROSE_SKIP_COMPILATION_OF_WAVE");

  // Build the AST used by ROSE
     SgProject* sageProject = frontend(newArgv);

     Doxygen::annotate(sageProject);
     Doxygen::correctAllComments(sageProject, criteriaForGeneratingOrUpdatingDocumenatation);
   
     //For the comments that are not already in a group, classify
     //them into groups according to the criteria in OrganizeAllComments
     OrganizeAllComments oac;
     oac.traverse(sageProject, preorder);
     oac.attachComments();

	 AddStringToDescription atd;
     atd.traverse(sageProject, preorder);


     //Sort all comments according to the criteriain sort_comments
     sortComments(sageProject, sort_comments);

	 //Will generate the ROSE documentation, but using the filtering criteria in
	 //criteriaForUnparsingNewDocumentation it will not unparse Documentation for
	 //some classes.
     Doxygen::unparse(sageProject,criteriaForUnparsingNewDocumentation );
	 //Will only unparse the documentation for the Binary stuff in ROSE according
	 //to the criteria in unparseBinaryDocumentation
     Doxygen::unparse(sageProject,unparseBinaryDocumentation );

   }

