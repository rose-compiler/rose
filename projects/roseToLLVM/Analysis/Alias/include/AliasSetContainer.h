#ifndef _ALIASSETCONTAINER_H
#define _ALIASSETCONTAINER_H

#include <set>
#include <vector>
#include <map>
#include <rose.h>

/*
 * rose.h should be included before LLVM includes
 */

#include <llvm/Function.h>
#include <llvm/Module.h>
#include <utility>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <rosetollvm/Control.h>
#include <rosetollvm/LLVMVisitor.h>

enum AliasType {May = 2, Must = 1, No = 0};

namespace AliasDataSet 
{
    typedef std::set<std::string> AliasRef;
    typedef std::set<SgNode*> AliasNodes;

    typedef std::pair<AliasType, AliasRef> AliasSet;
    typedef std::pair<AliasType, AliasNodes> RoseAliasSet;

    typedef std::vector<AliasSet> AliasSetList;
    typedef std::vector<RoseAliasSet> RoseAliasSetList;
}

/*
 * AliasSetContainer holds all alias sets in a function
 */

class AliasSetContainer
{
    std::string _functionname;       // to associate sets with a function

    /*
     * LLVM Pass String Write Format {AliasType, AliasReferences..}
     */

    std::string _aliaslocations;    

    AliasDataSet::AliasSetList _aliasSetList;
    AliasDataSet::RoseAliasSetList _roseAliasSetList;

    public:

    AliasSetContainer(std::string _fn_name );

    
    /*
     * use this function to add alias sets in LLVM Pass
     */ 
     
    void addaliasinglocations(std::string _alias);        

    /*
     * Parse LLVM's Alias Analysis output
     */

    void parseAliasSet();

    /*
     * returns true or false for if a set contains an element
     */
    bool isPresent(int index, std::string _element); 

    /*
     * returns true of false for a set containing sgnode
     */

    bool isSgNodePresent(int index, SgNode *node);

    /*
     * returns size 
     */
    int getSize();

    /*
     * add sgnode to the set by index number
     */

    void addSgNode(int index, SgNode *node);

    /*
     * clone set for rose ast nodes
     */
     void initSets();

     /*
      * return set<sgnode*>
      */
     std::set<SgNode*> getSgNodeSet(int index);

     /*
      * return type of alias set at index
      */

     AliasType getAliasType(int index);


    // for debugging
    void print();

    private:
    
    void printAliasType(AliasType _type);

    std::string printRoseAliasNode(SgNode *node);
     
};

class AliasSetContainerList
{
    std::string _modulename;
    std::map<std::string, AliasSetContainer*>  _list;

    public:
        AliasSetContainerList(std::string _module_name);

        void addNewFunction(std::string _functionname ) ;

        AliasSetContainer* getContainer(std::string _functionname );

        ~AliasSetContainerList();

        void parseAliasSet();

        // for debugging purposes
        void print();

        bool isFunctionPresent(std::string _functionname);
};

#endif
