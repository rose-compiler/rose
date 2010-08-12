#include <doxygenGrammar.h>
#include <string>
#include <map>

class DoxygenEntry 
   {

     public:
          enum EntryType 
             {
               None,
               Class,
               Variable,
               Function
             };

     private:

	  //PS! An entry that has _type EntryType::None
	  //and where hasName() is true is a group start or end
	  //DoxygenComment*
          EntryType _type;

          DxNodeList *nodes;
          std::string *prototypeP;
          std::string *briefP;
          std::string *descriptionP;
          std::string *returnsP;
          std::string *deprecatedP;
          std::string *nameP;

          std::map<std::string, std::string*> paramMap;
          DxNodeList::iterator afterLastParam;

          void extractParts();
          void addParameter(std::string name);

     public:
          DoxygenEntry(std::string comm);
          DoxygenEntry(EntryType type);

          EntryType &type();

          bool hasPrototype();
          std::string &prototype();
          bool hasBrief();
          std::string &brief();
          void set_brief(std::string newString);

          //The decsription variable contains description and todo
          bool hasDescription();
          std::string &description();
          void set_description(std::string newString);

          bool hasReturns();
          std::string &returns();
          bool hasDeprecated();
          std::string &deprecated();
       
          bool hasName();
	  //name() is only a string other than ""
	  //when the entry belongs to a group
	  //start or end DoxygenComment* 
          std::string &name();

          bool hasParameter(std::string x);
          std::string &parameter(std::string x);

          std::string unparse();

          static bool isDoxygenEntry(std::string comm);
          static std::string getProtoName(std::string proto);
   };
