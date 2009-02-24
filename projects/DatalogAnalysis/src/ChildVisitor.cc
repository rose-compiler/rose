#include <rose.h>
#include "ChildVisitor.hh"
#include "DBFactory.hh"

using namespace std;

ChildVisitor::ChildVisitor()
{
    Domain *sig[2]= {nodes,nodes};
    string doc[2]= {"child node", "parent node"};

    childRelation = db->createRelation( "childRelation", 2, sig, doc );
}

void
ChildVisitor::visit( SgNode *node )
   {
     typedef vector<pair<SgNode*,string> > childList;
     childList children = node->returnDataMemberPointers();
     childList::iterator i = children.begin();
     while (i != children.end())
        {
          if (i->first != NULL)
             {
               Element *tuple[2];

               tuple[0] = getElement(i->first);
               tuple[1] = getElement(node);

               childRelation->insert(tuple);
             }

          i++;
        }
   }
