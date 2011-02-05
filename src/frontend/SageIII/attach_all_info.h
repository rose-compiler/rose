#ifndef _ATTACH_ALL_INFO_H_
#define _ATTACH_ALL_INFO_H_

#if 0
struct wrap_data_used_by_AttachPreprocessingInfoTreeTrav_t
   {
     SgLocatedNode *previousLocNodePtr;
#if 0
  // DQ (11/30/2008): Moved currentListOfAttributes to inherited attribute
     ROSEAttributesList *currentListOfAttributes;
     int sizeOfCurrentListOfAttributes;
#endif
  // wrap_data_used_by_AttachPreprocessingInfoTreeTrav_t(SgLocatedNode* node=NULL, ROSEAttributesList* preprocInfo=NULL, int len=0);
     wrap_data_used_by_AttachPreprocessingInfoTreeTrav_t(SgLocatedNode* node=NULL);
   };
#else
// DQ (12/3/2008): This is the data that we have to save (even though the currentListOfAttributes has been moved to the inherited attribute)
struct wrap_data_used_by_AttachPreprocessingInfoTreeTrav_t
   {
     SgLocatedNode *previousLocNodePtr;
     ROSEAttributesList *currentListOfAttributes;
     int sizeOfCurrentListOfAttributes;
     wrap_data_used_by_AttachPreprocessingInfoTreeTrav_t(SgLocatedNode* node=NULL, ROSEAttributesList* preprocInfo=NULL, int len=0);
   };
#endif

class AttachAllPreprocessingInfoTreeTrav : public AttachPreprocessingInfoTreeTrav
   {
     private:
          std::string src_file_name;
          SgFile * sage_file;

       /* map: key = filename, value = a wrapper for the data used in AttachPreprocessingInfoTreeTrav. */
          std::map<int, wrap_data_used_by_AttachPreprocessingInfoTreeTrav_t> map_of_all_attributes;

       /* map: key = filename, value = the first node in AST from the file (could be NULL) */
       /* std::map<std::string, SgNode*> map_of_first_node; */
       /* I need to keep the order for each file when it is discovered from AST */
          int nFiles;                   /* total number of files involved */
          std::map<int, int> map_of_file_order;

          std::vector<SgNode*> array_of_first_nodes;

       // This is hepful in limiting the amount of data saved and files processed
       // within the handling of a large application that include system header files.
       // Basically we want to avoid processing comments in system header files 
       // because they are not relevant.
       // Holds paths to exclude when getting all commments and directives
          std::vector<std::string> pathsToExclude;
          bool lookForExcludePaths;

       // Holds paths to include when getting all comments and directives
          std::vector<std::string> pathsToInclude;
          bool lookForIncludePaths;

       /* set the first node for the file (may resize the
          array_of_first_nodes); return false means the first node for the
          file has already existed and keep the previous first node. */
          bool add_first_node_for_file(const int, SgNode* n, int pos);

       /* it returns the first node after file "fn"; return <NULL,
          anything> iff no node in the whole array_of_first_nodes for
          attaching preprocessing info; "hintfororder" is the number of
          handled files before file "fn". */
          std::pair<SgNode*, PreprocessingInfo::RelativePositionType> get_first_node_for_file(const int fn, int hintfororder);

     public:
          AttachAllPreprocessingInfoTreeTrav(SgFile * sagep);

       /* Pi: To make the way for attaching preproc info the same as
          before, we basically call the same functions in
          AttachPreprocessingInfoTreeTrav, although we need to set the data
          used by them before calling them, and update the data after they
          use it. */
          AttachPreprocessingInfoTreeTraversalInheritedAttrribute evaluateInheritedAttribute( SgNode *n,        AttachPreprocessingInfoTreeTraversalInheritedAttrribute inh);
          AttachPreprocessingInfoTreeTraversalSynthesizedAttribute evaluateSynthesizedAttribute( SgNode *n,AttachPreprocessingInfoTreeTraversalInheritedAttrribute inh, SubTreeSynthesizedAttributes st);

       /* to handle those files which contain no IR nodes. */
     private:
          std::istream & dependancies;  /* use cin for now, better to run ROSE internally to get the list. TODO. */
     public:
       /* attach preproc info from files containing no IR nodes: assume the
          order of the list of dependency files are in the correct order,
          attach such preproc info to the first node from the files after
          the current file. TODO: still have a problem if no files after
          the current file. */
      //  bool attach_left_info();

       // DQ (10/27/2007): Added display function to output information gather durring the collection of 
       // comments and CPP directives across all files.
          void display(const std::string & label) const;
   };

#if 0
// DQ (10/27/2007): This is not used!

void attachAllPreprocessingInfo1(SgProject* sagep, const char * dep); /* dep is the file containing a list of dependencies. */
/* Alg (Too slow): (1) use appropriate shell commands to get a list of
   dependencies (in a certain format) (could be automated based on the
   command line options stored in sagep); (2) suppose n dependencies:
   traverse the whole AST n times, each time set current file name in
   the SgFile node to a different dependency before traversal (a hack
   based on evaluateInheritedAttribute(...) in class
   AttachPreprocessingInfoTreeTrav */

// DQ (4/19/2006): This is now placed into the attachPreprocessingInfo(SgFile*) function!
// void attachAllPreprocessingInfo(SgFile* sagep);
#endif

#endif /* _ATTACH_ALL_INFO_H_ */
