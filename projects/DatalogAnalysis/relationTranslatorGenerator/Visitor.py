from string import Template

#This file contains the templated code which is used to generate the
#Relation Visitor


#The skeleton for the RelationVisitor header file
Header= Template(
"""
/*
 * Generated Code!  Do not Edit!
 */

#ifndef RELATIONVISITOR_HH
#define RELATIONVISITOR_HH

#include <rose.h>
#include "common.hh"
#include "Relation.hh"


class RelationVisitor : public ROSE_VisitTraversal {
public:
    RelationVisitor();
    void visit( SgNode *n );

private:
$private_decls

};


#endif
"""
)

#The skeleton for the RelationVsiitor source file
Source= Template (
"""
/*
 * Generated Code!  Do not Edit!
 */

#include "RelationVisitor.hh"
#include "DBFactory.hh"

extern Domain *nodes;

Element *getElement( SgNode *n );

RelationVisitor::RelationVisitor()
{
$constructor
}

void
RelationVisitor::visit( SgNode * n )
{
$visit
}

"""
)


#How to visit a node for a specific Relation
NodeVisit= Template(
"""
    {
        $type *node= dynamic_cast<$type *>(n);
        if ( node != NULL $pred) {
            Element *tuple[$arity];
$set_tuple
            $relation->insert(tuple);
        }
    }
"""
)

#How to visit a node for a list Relation
ListVisit= Template(
"""
    {
        $type *node= dynamic_cast<$type *>(n);
        if (node != NULL $pred) {
            Element *parent= getElement(node);
            vector<$ctype *> &l= $list;
            if ( l.begin() != l.end() ) {
                Element *tuple[2];
                tuple[1]= getElement(*l.begin());
                tuple[0]= parent;
                $first_relation->insert(tuple);
            }

            Element *last= NULL;
            for (   vector<$ctype*>::iterator i= l.begin();
                    i != l.end();
                    i++
                )
            {
                Element *cur= getElement( *i );
                if (last) {
                    Element *tuple[3];
                    tuple[0]= cur;
                    tuple[1]= last;
                    tuple[2]= parent;
                    $next_relation->insert(tuple);
                }
                last= cur;
            }
        }
    }
"""
)


# the relation creation for use within the constructor
NodeInit= Template(
"""
    {
        Domain *sig[$arity];
        for (int i= 0; i< $arity; i++)
            sig[i]= nodes;

        string doc[$arity];
$set_doc
        $relation= db->createRelation( "$relation", $arity, sig, doc );
    }
"""
)

# the relation creation for list like construction
ListInit= Template(
"""
    {
        Domain *sig[3]= { nodes, nodes, nodes };
        string doc[3];

        doc[0]= "first child";
        doc[1]= "parent";

        $first_relation= db->createRelation( "$first_relation", 2, sig, doc );

        doc[0]= "next";
        doc[1]= "prev";
        doc[2]= "parent";

        $next_relation= db->createRelation( "$next_relation", 3, sig, doc );
    }
"""
)

