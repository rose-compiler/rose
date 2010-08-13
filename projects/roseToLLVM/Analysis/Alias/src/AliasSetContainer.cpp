#include <AliasSetContainer.h>
#include <map>
#include <string>
#include <iostream>


/*
 * Defintion for AliasSetContainer
 */

AliasSetContainer::AliasSetContainer(std::string _fn_name)
{
    _functionname = _fn_name;
}

void AliasSetContainer::addaliasinglocations(std::string _locations)
{
    _aliaslocations = _locations;
}

AliasSetContainerList::AliasSetContainerList(std::string _module)
{
    _modulename = _module;
}

void AliasSetContainer::printAliasType(AliasType _type)
{
    std::cout << "Alias Type : ";

    if(_type == May) {
        std::cout << "May" << std::endl;
    }
    else if(_type == Must) {
        std::cout << "Must" << std::endl;
    }
    else
        std::cout << "No" << std::endl;
}

std::string AliasSetContainer::printRoseAliasNode(SgNode *node)
{
    std::string class_name = node->class_name();
    std::string value;

/*    if(!class_name.compare("SgPointerDerefExp") || !class_name.compare("SgPntrArrRefExp")) {
        if(node->attributeExists(Control::LLVM_REFERENCE_NAME)) {
            value = ((StringAstAttribute*)node->getAttribute(Control::LLVM_REFERENCE_NAME))->getValue();
        }
    }
    else if(!class_name.compare("SgInitializedName")) {
         if(node->attributeExists(Control::LLVM_NAME)) {
            value = ((StringAstAttribute*)node->getAttribute(Control::LLVM_NAME))->getValue();
         }
    }
    return value; */
    return node->unparseToString();
}

void AliasSetContainer::print()
{
    std::cout << "Number of Alias Sets : " << _aliasSetList.size() << std::endl;

    AliasDataSet::RoseAliasSetList::iterator I, E;
    AliasDataSet::AliasNodes::iterator sI, sE;

    for(I = _roseAliasSetList.begin(), E = _roseAliasSetList.end(); I != E; ++I) {
        printAliasType(I->first);

        std::cout << "{";

        AliasDataSet::AliasNodes &aRef = I->second;

        for(sI = aRef.begin(), sE = aRef.end(); sI != sE; ++sI) {
            SgNode *node = *sI;
            std::cout <<  printRoseAliasNode(node) <<",";
        }

        std::cout << "}" << std::endl;

     }
}

void AliasSetContainer::parseAliasSet()
{
    assert( !_aliaslocations.empty() );
   
    std::vector<std::string> _allSetsinThisFunc ;

    boost::split (_allSetsinThisFunc, _aliaslocations, boost::is_any_of("{}"));

    std::vector<std::string>::iterator I;
    std::string _aliastype, _aliasref;
    std::vector<std::string> _allRefsinThisSet;


    enum AliasType _aliasType;


    for( I = _allSetsinThisFunc.begin(); I != _allSetsinThisFunc.end(); ++I) {
        _aliasref = I->data();

        /*
         * Empty lines between } and {
         */

        boost::trim(_aliasref);
        if(_aliasref.empty()) continue;

        /*
         * Non-empty lines for each alias set 
         */

        boost::split(_allRefsinThisSet, _aliasref, boost::is_any_of(","));

        /*
         * Needs to be alteast 2
         * one for type and 2 or more for aliasing references
         */

        if(_allRefsinThisSet.size() > 2) {
            
            _aliastype = _allRefsinThisSet.back(); 
            _allRefsinThisSet.pop_back();

            if(! _aliastype.compare("May")) {
                _aliasType = May;
            }
            else if(! _aliastype.compare("Must")) {
                _aliasType = Must;
            }
            else {
                _aliasType = No;
            }

            AliasDataSet::AliasRef _aRefs;
           _aRefs.insert(_allRefsinThisSet.begin(), _allRefsinThisSet.end());
            AliasDataSet::AliasSet _aSet = make_pair(_aliasType, _aRefs);
            _aliasSetList.push_back(_aSet);
        }
    }
}

int AliasSetContainer::getSize()
{
    return _aliasSetList.size();
}

void AliasSetContainer::initSets()
{
    int size = _aliasSetList.size();
    _roseAliasSetList.reserve(size);

    AliasType _aType;

    for(int i = 0; i < size; ++i) {
       _aType = _aliasSetList[i].first;
       AliasDataSet::AliasNodes _anodes;
       AliasDataSet::RoseAliasSet _rosealiasset = std::make_pair(_aType, _anodes);
       _roseAliasSetList.push_back(_rosealiasset);
    }
}

bool AliasSetContainer::isPresent(int index, std::string _element)
{
    AliasDataSet::AliasRef &set = _aliasSetList.at(index).second;
    if(set.find(_element) != set.end())
        return true;
    else
        return false;
}

bool AliasSetContainer::isSgNodePresent(int index, SgNode *node)
{
    AliasDataSet::AliasNodes &ref = _roseAliasSetList[index].second;

    if(ref.find(node) != ref.end())
        return true;
    else
        return false;
}

std::set<SgNode*> AliasSetContainer::getSet(int index)
{
    AliasDataSet::AliasNodes ref = _roseAliasSetList[index].second;
    return ref;
}

void AliasSetContainer::addSgNode(int index, SgNode *node)
{
    AliasDataSet::AliasNodes &_nodeset = _roseAliasSetList[index].second;
    _nodeset.insert(node);
}

AliasType AliasSetContainer::getAliasType(int index)
{
    AliasType type = _roseAliasSetList[index].first;
    return type;
}



/*
 * Definition for AliasSetContainerList
 */

void AliasSetContainerList::addNewFunction(std::string _functionname ) 
{
    AliasSetContainer *_ascontainer = new AliasSetContainer(_functionname);
    _list[_functionname] = _ascontainer;
//  _list.insert(std::make_pair(_functionname, _ascontainer));            
}
        
AliasSetContainer* AliasSetContainerList::getContainer(std::string _functionname )
{
    return _list.find(_functionname)->second;
}

AliasSetContainerList:: ~AliasSetContainerList()
{
    // Cleanup
    std::map<std::string, AliasSetContainer*>::iterator I;
    for(I = _list.begin(); I != _list.end(); ++I) {
        delete I->second;
    }
       
    _list.erase(_list.begin(), _list.end());
}     

void AliasSetContainerList::parseAliasSet()
{
    std::map<std::string, AliasSetContainer*>::iterator I;
    for(I = _list.begin(); I != _list.end(); ++I) {
//        std::cout << I->first << std::endl;
        I->second->parseAliasSet();
        std::cout << std::endl;
    }
}

void AliasSetContainerList::print()
{
    std::map<std::string, AliasSetContainer*>::iterator I;
    for(I = _list.begin(); I != _list.end(); ++I) {
        I->second->print();
    }
}

bool AliasSetContainerList::isFunctionPresent(std::string _functionname)
{
    bool res = false;
    if(_list.find(_functionname) != _list.end()) {
        res = true;
    }
    return res;
}
