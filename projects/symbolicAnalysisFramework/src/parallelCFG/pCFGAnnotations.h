#ifndef _ANNOTATIONSUTIL_H
#define _ANNOTATIONSUTIL_H

/*
 * helper annotations to build pcfg
 * pcfg annotation types
 * pcfg annotation parser
 */

#include "rose.h"
#include <string>
#include <vector>

using namespace std;

/*
 * annotation types
 */

enum pcfg_pragma_type {
    pcfg_split,
    pcfg_merge,
    pcfg_match,
    pcfg_none
};

/*
 * Basic Annotation class
 */
class Annotation : public AstAttribute
{
    // type of the annotation
    pcfg_pragma_type annot_type;
       
    public:

    // default constructor
    Annotation(pcfg_pragma_type _type) : annot_type(_type)
    { }
    
    virtual pcfg_pragma_type getType() {
        return annot_type;
    }

    Annotation(const Annotation& _annot) {
        annot_type = _annot.annot_type;   
    }

    void addAnnotation(SgNode* node) {
        assert(node != NULL);
        node->addNewAttribute("pCFGAnnotation", this);
    }

    virtual ~Annotation() { }

};

// Associated with rank dependent condition
// tells pcfg iterator how many process sets to split at node
// 
class pcfgSplitAnnotation: public Annotation
{
    // list of names for process set
    // index correspond to pSet
    vector<string> names;
    int size;

    public:
    pcfgSplitAnnotation(int _size, pcfg_pragma_type annot_type) : Annotation(annot_type)
    {
        size = _size;
    }

    pcfgSplitAnnotation(const pcfgSplitAnnotation& _split_annot) : Annotation(_split_annot)
    {
        names = _split_annot.names;
        size = _split_annot.size;        
    }

    // insert at 
    void insert(string _name)
    {
        names.push_back(_name);
    }

    string getPSetName(int pset)
    {
        return names[pset];
    }

    int getPartitionSize()
    {
        return size;
    }

    vector<string>& getpSetNames()
    {
        return names;
    }
        
    std::string toString();    
};

class pcfgMergeAnnotation : public Annotation
{
    public:
    pcfgMergeAnnotation(pcfg_pragma_type annot_type) : Annotation(annot_type)
    {
    }
    ~pcfgMergeAnnotation() {}
};


class pcfgMatchAnnotation : public Annotation
{
    pair<string, int> source;
    pair<string, int> target;

    public:
    pcfgMatchAnnotation(pcfg_pragma_type annot_type) : Annotation(annot_type)
    {        

    }

    void setSource(string _psetname, int _psetindex)
    {
        source = make_pair(_psetname, _psetindex);
    }

    void setTarget(string _matchname, int _matchindex)
    {
        target = make_pair(_matchname, _matchindex);
    }

    pair<string, int> getSource()
    {
        //ROSE_ASSERT(source);
        return source;
    }

    pair<string, int> getTarget()
    {
        //ROSE_ASSERT(target);
        return target;
    }

    std::string toString();

    ~pcfgMatchAnnotation() { }
};

class pcfgAnnotationParser
{
    public:
    // interface to the parser
    string match_any_string_constant();
    void parseAnnotate(string, SgNode*);
    Annotation* parseSplitAnnotation();
    Annotation* parseMatchAnnotation();
};

class PragmaParse : public AstSimpleProcessing
{
    pcfgAnnotationParser pcfgAParser;
    
    public:
    PragmaParse() { }

    void visit(SgNode*);
};

class PragmaParseTest : public AstSimpleProcessing
{
    public:
    PragmaParseTest() { }
    void visit(SgNode*);
};


#endif 
