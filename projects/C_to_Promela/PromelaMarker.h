#ifndef PromelaMarker_H_
#define PromelaMarker_H_

#include <set>
#include <string>
#include <list>
/* ! \class PromelaMarker

   This class scans throught the AST for two different pragmas: First, a
   pragma calls "SliceFunction" which indicates that the immediately
   proceeding function is the function that the slicing algorithm should
   target. Second, it finds the statement immediately proceeding the pragma
   "SliceTarget" for use as the slicing criterion.

   Each of these pragmas are assumed to appear only once in the file.

   @todo Allow slicing criterion to be a set of statements rather than an
   individual statement.

 */

class PromelaMarker:public AstSimpleProcessing
{

  public:

    PromelaMarker();
		void traverse(SgNode* node, Order treeTraversalOrder);
/*		std::list<SgNode *> getTargetList()
		{
			return modelTargetList;;
		}-*/
  protected:
    virtual void visit(SgNode * node);
		const std::string toProMeLaString;
		const std::string allToProMeLaString;
		bool markAllForTransformation;
//		std::list<SgNode *>modelTargetList;
};


#endif
