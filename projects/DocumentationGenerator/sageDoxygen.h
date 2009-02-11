#include <commentObj.h>
// #include <rose.h>
#include <string>
#include <map>

class DoxygenFile;
class DoxygenGroup;


class DoxygenComment 
   {

     public:
       /*! Create a new, blank DoxygenComment attached to the given node. */
          DoxygenComment(SgLocatedNode *node);

       /*! Create a DoxygenComment associated with an existing comment. */
          DoxygenComment(SgLocatedNode *node, PreprocessingInfo *comment);
          virtual ~DoxygenComment();
          DoxygenEntry entry;
          PreprocessingInfo *originalComment;
          SgLocatedNode *originalNode;
          DoxygenFile *originalFile;

          static bool isDoxygenComment(PreprocessingInfo *comm);

          void attach(SgLocatedNode *newParent, AttachedPreprocessingInfoType::iterator pos);
          void attach(SgLocatedNode *newParent);
          void attach(DoxygenFile *newParent);
          void attach(DoxygenFile *newParent, DoxygenGroup *newGroup);

       /*! Updates the originalComment using information in entry */
          void updateComment();
          void addUndocumentedParams(SgFunctionDeclaration *fn);

   };

class DoxygenFile 
   {
  // DQ: This is used for doxygen comments held in separate files.
     public:
          DoxygenFile(SgLocatedNode *commentParent);
          DoxygenFile(SgProject *prj, std::string filename);
          std::string name();
          std::string unparse();
          bool hasGroup(std::string name);
          DoxygenGroup *group(std::string name);

	  //AS(090707) Support sorting of comments according to a
	  //user-defined order
	  void sortComments( bool (*pred)(DoxygenComment*,DoxygenComment*) );
          void createGroup(std::string name);
          DoxygenGroup *findGroup(DoxygenComment *comm);

     private:
          SgLocatedNode *commentParent;
          std::map<std::string, DoxygenGroup *> groups; // should not be public (needed for building object)

          friend class DoxygenComment;
          friend struct Doxygen;

   };

class DoxygenCommentListAttribute : public AstAttribute 
   {
  // DQ: These are used for where the doxygen comments are held in the source code.
     public:
          std::list<DoxygenComment *> commentList;        

   };

struct Doxygen 
   {
     static std::string getProtoName(SgDeclarationStatement *st);
     static std::string getDeclStmtName(SgDeclarationStatement *st);
     static std::string getQualifiedPrototype(SgNode *node);
     static bool isRecognizedDeclaration(SgDeclarationStatement *n);
     static SgDeclarationStatement *getDoxygenAttachedDeclaration(SgDeclarationStatement *ds);
     static std::list<DoxygenComment *> *getCommentList(SgDeclarationStatement *ds);
     static std::map<std::string, DoxygenFile *> *getFileList(SgProject *prj);

     static bool isGroupStart(PreprocessingInfo *p);
     static bool isGroupEnd(PreprocessingInfo *p);

     static void annotate(SgProject *n);
     static void unparse(SgProject *p , bool (*shouldBeUnparsed)(DoxygenFile*, std::string& fileName) = NULL  );

	 //The correctAllComments function will update all doxygen comments and generate new doxygen comments for
	 //constructs that has not been documented. The function provided as the second argument,
	 // shouldHaveDocumentation, will return false if this should not be performed for a SgNode* or true
	 //otherwise.
     static void correctAllComments(SgProject *prj, bool (*shouldHaveDocumentation)(SgNode*) = NULL    );
     static void lint(SgNode *n);

     static void parseCommandLine(std::vector<std::string>& argvList);

     struct DoxygenOptions
        {
          DoxygenOptions() : outputDirectory(".") {}

          std::string classNameTemplate;
          std::string functionNameTemplate;
          std::string variableNameTemplate;
          std::string updateScript;
          std::string outputDirectory;
          bool createNewFiles;
          bool inplace;
        };

     static DoxygenOptions *options;

     static DoxygenFile *destinationFile(SgProject *prj, SgDeclarationStatement *decl);
     static std::string destinationFilename(SgDeclarationStatement *decl);
   };

class DoxygenGroup 
   {
  // DQ: This is used for doxygen comments held in separate files.
     public:
          std::string getName();

     private:
          PreprocessingInfo* groupStart;
          PreprocessingInfo* groupEnd;
          DoxygenComment *comment;

          friend class DoxygenFile;
          friend struct Doxygen;
          friend class DoxygenComment;
   }
;

extern std::map<const PreprocessingInfo*, DoxygenComment* > commentMap;

