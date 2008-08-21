#include "common.hh"
#include "DBFactories/TupleFactory.hh"
#include "Domain.hh"
#include "Relation.hh"
#include <sstream>
#include <ext/hash_map>
// #include "RelationVisitor.hh"
#include "../relationTranslatorGenerator/output/RelationVisitor.hh"
#include "CFGVisitor.hh"
#include "ChildVisitor.hh"

DBFactory *db;
Domain *nodes;
Domain *numbers;
Element *nullElement;

Element *getElement( SgNode * );

/* A simple visitor for creating elements for all SgNodes
 */
class DomainV : public ROSE_VisitTraversal {
public:
    void visit( SgNode *n );
};

/*create the globals*/
void initGlobals()
{
    db= new TupleFactory();
    nodes= db->createDomain("node");
    numbers= db->createDomain("number");
    numbers->createElement("0");
    nullElement= getElement(NULL);
}

/* The the element representing some number.
 * This must instantiate elements as necessary.
 */
Element *getNumber( unsigned int n )
{
    static unsigned int max= 0;
    if (n<=max) {
        ostringstream str;
        str << n;
        return numbers->findElement(str.str());
    }

    Element *last = NULL;
    for (unsigned int i= max+1; i <= n; i++) {
        ostringstream str;
        str << i;
        last= numbers->createElement(str.str());
    }
    max=n;

    return last;
}

/* Currently all SgNodes are in one domain, nodes. */
Domain *getDomain( SgNode *n )
{
    return nodes;
}


/* Create a string representation of a node */
string
getNodeString( SgNode *n )
{
    ostringstream str;

    SgLocatedNode *l= dynamic_cast<SgLocatedNode*>(n);
    SgType *t= dynamic_cast<SgType*>(n);
    SgSymbol *s=  dynamic_cast<SgSymbol*>(n);
    SgName *name= dynamic_cast<SgName*>(n);

    if (l) {
        Sg_File_Info *info= n->get_file_info();
        str << info->get_filenameString() << ':';
        str << info->get_line() << ':';
        str << info->get_col();
    } else if (t) {
        str << t->unparseToString();
    } else if (s) {
        str << s->get_name().str();
    } else if (name) {
        str << "name: " << name->str();
        return str.str();
    } else {
        str << (void*)n;
    }


    str << "  " << n->class_name();

    return str.str();
}


struct hashSgNodePtr {
      size_t operator()(SgNode *x) const
      { return reinterpret_cast<size_t>(x); }
};

/* Rather than use attributes, elemtns are stored in this map */
typedef __gnu_cxx::hash_map< SgNode *, Element *, hashSgNodePtr > emap;

emap elements;

/* Get the element for an SgName.  Notice that distinct objects with
 * the same name use the same Element.
 */
Element*
getElementN( SgName *n )
{
    string s= getNodeString(n);
    Element *e= getDomain( n )->findElement( s );
    if (!e) {
        e= getDomain( n )->createElement( s );
    }
    return e;
}

/*Get the Element for any SgNode*/
Element*
getElement( SgNode *n )
{
    if (n == NULL) {
        if (nullElement)
            return nullElement;
        
        nullElement= nodes->createElement("null");
        return nullElement;
    }

    SgName *name= dynamic_cast<SgName *>(n);
    if (name)
        return getElementN( name );

    emap::iterator itr= elements.find(n);
    if (itr == elements.end()) {
        Element *e= getDomain( n )->createElement( getNodeString(n) );
        elements[n]= e;
        return e;
    } else {
        return elements[n];
    }
#if 0
    if (n->attributeExists("element")) {
        return static_cast<ElementAttribute*>(n->getAttribute("element"))->e;
    } else {
        Element *e= getDomain( n )->createElement(getNodeString(n));
        ElementAttribute *ea= new ElementAttribute( e );
        n->setAttribute("element", ea);
        return e;
    }
#endif
}


/* These nodes are treated as lists.
 * This is no longer used, since we have moved to a
 * generated RelationVisitor
 */
bool
isList( SgNode *n )
{
    if (dynamic_cast<SgFileList*>(n) ) return true;
    if (dynamic_cast<SgDirectoryList*>(n)) return true;
    if (dynamic_cast<SgForInitStatement*>(n)) return true;
    if (dynamic_cast<SgCatchStatementSeq*>(n)) return true;
    if (dynamic_cast<SgEnumDeclaration*>(n)) return true;
    if (dynamic_cast<SgFunctionParameterList*>(n)) return true;
    if (dynamic_cast<SgCtorInitializerList*>(n)) return true;
    if (dynamic_cast<SgGlobal*>(n)) return true;
    if (dynamic_cast<SgBasicBlock*>(n)) return true;
    if (dynamic_cast<SgNamespaceDefinitionStatement*>(n)) return true;
    if (dynamic_cast<SgTemplateInstantiationDefn*>(n)) return true;
    if (dynamic_cast<SgClassDefinition*>(n)) return true;
    if (dynamic_cast<SgExprListExp*>(n)) return true;
    if (dynamic_cast<SgSymbolTable*>(n)) return true;
    return false;
}


void
DomainV::visit( SgNode *n )
{
    getElement(n);
}



int main( int argc, char **argv )
{
    int myargc= argc+1;
    char **myargv= (char**)malloc( sizeof (char*) * (myargc) );
    myargv[0]= argv[0];
    myargv[1]= (char*)"--edg:no_warnings";
    for (int i= 1; i < argc; i++ ) {
        myargv[i+1]= argv[i];
    }

    SgProject *project= frontend(myargc,myargv);
    ROSE_ASSERT( project != NULL );

//   generatePDF(*project);
//    AstTests::runAllTests(project);

    cerr << "**********************************************************";
    cerr << endl << "BEGIN RELATION GENERATION" << endl << endl;

    initGlobals();

    DomainV dv;
    dv.traverseMemoryPool();

    RelationVisitor rv;
    rv.traverseMemoryPool();

    CFGVisitor cv;
    cv.traverseMemoryPool();

    ChildVisitor childvisitor;
    childvisitor.traverseMemoryPool();

    db->outputDB( "dump" );
}

