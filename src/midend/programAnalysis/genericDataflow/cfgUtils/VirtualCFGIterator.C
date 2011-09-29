#include "variables.h"
#include "virtualCFG.h"
#include "VirtualCFGIterator.h"

#include <list>
using std::list;
#include <vector>
using std::vector;
#include <set>
using std::set;
#include <string>
using std::string;
#include <iostream>
using std::cout;
using std::ostringstream;

namespace VirtualCFG{
        
/******************************
 ********** ITERATOR **********
 ******************************/
        
iterator::iterator() {
        initialized     = false;
}

iterator::iterator(const DataflowNode &start) 
{
        initialized     = true;
        remainingNodes.push_front(start);
        visited.insert(start);
}

void iterator::init(const DataflowNode &start) {
        initialized     = true;
        remainingNodes.push_front(start);
        visited.insert(start);
}


// returns true if the given DataflowNode is in the remainingNodes list and false otherwise
bool iterator::isRemaining(DataflowNode n)
{
        ROSE_ASSERT(initialized);
        for(list<DataflowNode>::const_iterator it=remainingNodes.begin(); it!=remainingNodes.end(); it++)
        {
                // if in is currently n remainingNodes, say so
                if(*it == n) return true;
        }
        // n is not in remainingNodes
        return false;
}

// advances this iterator in the given direction. Forwards if fwDir=true and backwards if fwDir=false.
// if pushAllChildren=true, all of the current node's unvisited children (predecessors or successors, 
//    depending on fwDir) are pushed onto remainingNodes
void iterator::advance(bool fwDir, bool pushAllChildren)
{
        ROSE_ASSERT(initialized);
        /*printf("   iterator::advance(%d) remainingNodes.size()=%d\n", fwDir, remainingNodes.size());
        cout<<"        visited=\n";
        for(set<DataflowNode>::iterator it=visited.begin(); it!=visited.end(); it++)
                cout << "            <"<<it->getNode()->class_name()<<" | "<<it->getNode()<<" | "<<it->getNode()->unparseToString()<<">\n";*/
        if(remainingNodes.size()>0)
        {
                // pop the next CFG node from the front of the list
                DataflowNode cur = remainingNodes.front();
                remainingNodes.pop_front();
                
                if(pushAllChildren)
                {
                        // find its followers (either successors or predecessors, depending on value of fwDir), push back 
                        // those that have not yet been visited
                        vector<DataflowEdge> nextE;
                        if(fwDir)
                                nextE = cur.outEdges();
                        else
                                nextE = cur.inEdges();
                        for(vector<DataflowEdge>::iterator it=nextE.begin(); it!=nextE.end(); it++)
                        {
                                DataflowNode nextN((*it).target()/* need to put something here because DataflowNodes don't have a default constructor*/);
                                if(fwDir) nextN = (*it).target();
                                else nextN = (*it).source();
                                        
                                /*cout << "      iterator::advance "<<(fwDir?"descendant":"predecessor")<<": "<<
                                                   "<"<<nextN.getNode()->class_name()<<" | "<<nextN.getNode()<<" | "<<nextN.getNode()->unparseToString()<<">, "<<
                                                   "visited="<<(visited.find(nextN) != visited.end())<<
                                                   " remaining="<<isRemaining(nextN)<<"\n";*/
                                
                                // if we haven't yet visited this node and don't yet have it on the remainingNodes list
                                if(visited.find(nextN) == visited.end() &&
                                        !isRemaining(nextN))
                                {
                                        //printf("   pushing back node <%s: 0x%x: %s> visited=%d\n", nextN.getNode()->class_name().c_str(), nextN.getNode(), nextN.getNode()->unparseToString().c_str(), visited.find(nextN)!=visited.end());
                                        remainingNodes.push_back(nextN);
                                }
                        }
                }
                
                // if we still have any nodes left remaining
                if(remainingNodes.size()>0)
                {
                        // take the next node from the front of the list and mark it as visited
                        //visited[remainingNodes.front()] = true;
                        visited.insert(remainingNodes.front());
                }
        }
}

void iterator::operator ++ (int)
{
        ROSE_ASSERT(initialized);
        advance(true, true);
}

bool iterator::eq(const iterator& other_it) const
{
        if(initialized != other_it.initialized) return false;
                
        // If both iterators are not initialized, they're equal
        if(!initialized) return true;
                
        //printf("iterator::eq() remainingNodes.size()=%d  other_it.remainingNodes.size()=%d\n", remainingNodes.size(), other_it.remainingNodes.size());
        if(remainingNodes.size() != other_it.remainingNodes.size()) return false;
        
        list<DataflowNode>::const_iterator it1, it2;
        // look to ensure that every CFG node in other_it.remainingNodes appears in remainingNodes
        
        for(it1=remainingNodes.begin(); it1!=remainingNodes.end(); it1++)
        {
                for(it2=other_it.remainingNodes.begin(); it2!=other_it.remainingNodes.end(); it2++)
                {
                        // if we found *it1 inside other_it.remainingNodes
                        if(*it1 == *it2)
                        {
                                //printf("              (*it1 == *it2)\n");
                                break;
                        }
                }
                
                // the two iterators are not equal if ...
                
                // the current node in remainingNodes was not found in other_it.remainingNodes
                if(it2!=other_it.remainingNodes.end()) 
                {
                        //printf("              it2!=other_it.remainingNodes.end()\n");
                        return false; 
                }
                        
                // or the two nodes do not have the same visited status in both iterators
                if((visited.find(*it1) == visited.end()) !=
                   (other_it.visited.find(*it1) == other_it.visited.end()))
                {
                        //printf("              (visited.find(*it1) == visited.end()) != (other_it.visited.find(*it1) == other_it.visited.end())\n");
                        return false;
                }
        }
        //printf("iterator::eq: returning true\n");
        
        return true;
}

bool iterator::operator==(const iterator& other_it) const
{
        return eq(other_it);
}

bool iterator::operator!=(const iterator& it) const
{
        return !(*this == it);
}
        
DataflowNode& iterator::operator * ()
{
        ROSE_ASSERT(initialized);
/*              printf("VirtualCFG::iterator::operator* remainingNodes.size()=%d\n", remainingNodes.size());
        printf("VirtualCFG::iterator::operator* remainingNodes.front()->getNode()=0x%x\n", remainingNodes.front().getNode());
        printf("VirtualCFG::iterator::operator* ===\n");*/
        return remainingNodes.front();
}

iterator iterator::begin(DataflowNode n)
{
        iterator newIter(n);
        return newIter;
}

iterator iterator::end()
{
        iterator blank;
        // Make sure that this iterator is initialized even though it is empty
        blank.initialized = true;
        return blank;
}

iterator::checkpoint::checkpoint(const list<DataflowNode>& remainingNodes, const set<DataflowNode>& visited)
{
        this->remainingNodes = remainingNodes;
        this->visited        = visited;
}

iterator::checkpoint::checkpoint(const iterator::checkpoint& that)
{
        this->remainingNodes = that.remainingNodes;
        this->visited        = that.visited;
}

string iterator::checkpoint::str(string indent)
{
        ostringstream outs;
//printf("VirtualCFG::iterator::checkpoint A\n"); fflush(stdout);
        outs << indent << "[VirtualCFG::iterator::checkpoint : \n"; //fflush(stdout);
//printf("VirtualCFG::iterator::checkpoint B, remainingNodes.size()=%d\n", remainingNodes.size()); fflush(stdout);
        for(list<DataflowNode>::iterator it=remainingNodes.begin();
            it!=remainingNodes.end(); )
        {
//printf("VirtualCFG::iterator::checkpoint C, (*it).getNode()=%p\n", (*it).getNode()); fflush(stdout);
                outs << indent << "    <"<<(*it).getNode()->class_name()<<" | "<<(*it).getNode()->unparseToString()<<">";
                it++;
                if(it!=remainingNodes.end()) outs << "\n";
        }
//printf("VirtualCFG::iterator::checkpoint D\n"); fflush(stdout);
        outs << "]";
        return outs.str();
}

// Returns a checkpoint of this iterator's progress.
iterator::checkpoint iterator::getChkpt()
{
        ROSE_ASSERT(initialized);
        iterator::checkpoint chkpt(remainingNodes, visited);
        return chkpt;
        //return new iterator::checkpoint::checkpoint(remainingNodes, visited);
}

// Loads this iterator's state from the given checkpoint.
void iterator::restartFromChkpt(iterator::checkpoint& chkpt)
{
        
        remainingNodes.clear();
        visited.clear();
        
        cout << "iterator::restartFromChkpt() chkpt.remainingNodes.size()="<<chkpt.remainingNodes.size()<<"\n";
        for(list<DataflowNode>::iterator it=chkpt.remainingNodes.begin();
            it!=chkpt.remainingNodes.end(); it++)
        {
                printf("    (*it).getNode() = %p\n", (*it).getNode());
                printf("    (*it).getNode() = <%s | %s>\n", (*it).getNode()->class_name().c_str(), (*it).getNode()->unparseToString().c_str());
        }
        
        remainingNodes = chkpt.remainingNodes;
        visited = chkpt.visited;
        // The iterator must become initialized because it is only possible to take a checkpoints of initialized iterators
        initialized = true;
}

string iterator::str(string indent)
{
        
        ostringstream outs;
        
        if(initialized) {
                outs << "[iterator:\n";
                outs << "    remainingNodes = \n";
                for(list<DataflowNode>::iterator it=remainingNodes.begin(); it!=remainingNodes.end(); it++)
                { outs << "        <"<<(*it).getNode()->class_name()<<" | "<<(*it).getNode()->unparseToString()<<" | "<<(*it).getIndex()<<"\n"; }
                
                outs << "    visited = \n";
                for(set<DataflowNode>::iterator it=visited.begin(); it!=visited.end(); it++)
                { outs << "        <"<<(*it).getNode()->class_name()<<" | "<<(*it).getNode()->unparseToString()<<" | "<<(*it).getIndex()<<"\n"; }
                
                outs << "]";
        } else {
                outs << "[iterator: Uninitialized]";
        }
                
        return outs.str();
}


/*****************************
 ******* BACK_ITERATOR *******
 *****************************/
        
void back_iterator::operator ++ (int)
{
        ROSE_ASSERT(initialized);
        advance(false, true);
}

/******************************
 ********** DATAFLOW **********
 ******************************/

dataflow::dataflow(const DataflowNode &terminator_arg) : terminator(terminator_arg)
{
        // Record that the terminator has been visited to ensure that it is never analyzed
        visited.insert(terminator);
}

dataflow::dataflow(const DataflowNode &start, const DataflowNode &terminator_arg): 
                iterator(start), terminator(terminator_arg)
{
        // Record that the terminator has been visited to ensure that it is never analyzed
        visited.insert(terminator);
        
        /*cout<<"        dataflow::dataflow() terminator=<"<<terminator.getNode()->class_name()<<" | "<<terminator.getNode()->unparseToString()<<"> visited=\n";
        for(set<DataflowNode>::iterator it=visited.begin(); it!=visited.end(); it++)
                cout << "            <"<<it->getNode()->class_name()<<" | "<<it->getNode()<<" | "<<it->getNode()->unparseToString()<<">\n";*/
        
        ROSE_ASSERT(start!=terminator);
}

void dataflow::init(const DataflowNode &start_arg, const DataflowNode &terminator_arg)
{
        ROSE_ASSERT(start_arg!=terminator_arg);
        // Use the init method to initialize the starting point to make sure that the object is recorded as being initialized
        iterator::init(start_arg);
        terminator = terminator_arg;
}
 
void dataflow::add(const DataflowNode &next)
{
        // If this dataflow iterator is not initialized, initialize it now since it will now have real state
        if(!initialized) initialized = true;
        
        // never add the terminator node
        if(next==terminator)
                return;
        
        // if next is not currently in remainingNodes, add it
        //cout << "dataflow::add() isRemaining()="<<isRemaining(next)<<" next = <"<<next.getNode()->class_name()<<" | "<<next.getNode()->unparseToString()<<">\n";
        if(!isRemaining(next))
        {
                set<DataflowNode>::iterator nextLoc = visited.find(next);
                //printf("    visited=%d\n", nextLoc != visited.end());
                if(nextLoc != visited.end())
                        visited.erase(visited.find(next));
                remainingNodes.push_back(next);
        }
        //printf("dataflow::add() remainingNodes.size()=%d\n", remainingNodes.size());
}

/*void dataflow::operator ++ (int)
{
        ROSE_ASSERT(initialized);
//      printf("dataflow::++() <<< remainingNodes.size()=%d\n", remainingNodes.size());
        advance(true, false);
//      printf("dataflow::++() >>> remainingNodes.size()=%d\n", remainingNodes.size());
}*/

dataflow::checkpoint::checkpoint(const iterator::checkpoint& iChkpt, const DataflowNode& terminator): 
        iChkpt(iChkpt), terminator(terminator) {}

dataflow::checkpoint::checkpoint(const dataflow::checkpoint &that): 
        iChkpt(that.iChkpt), terminator(that.terminator) {}

string dataflow::checkpoint::str(string indent)
{
        ostringstream outs;
        outs << indent << "[VirtualCFG::dataflow::checkpoint : \n"; //fflush(stdout);
//printf("VirtualCFG::dataflow::checkpoint A\n"); fflush(stdout);
        outs << indent << "    iterator = \n"<<iChkpt.str(indent+"    ")<<"\n";
//printf("VirtualCFG::dataflow::checkpoint \n"); fflush(stdout);
        outs << indent << "    terminator = <"<<terminator.getNode()->class_name()<<" | "<<terminator.getNode()->unparseToString()<<">]";
//printf("VirtualCFG::dataflow::checkpoint C\n"); fflush(stdout);
        return outs.str();
}

// Returns a checkpoint of this iterator's progress.
dataflow::checkpoint dataflow::getChkpt()
{
        ROSE_ASSERT(initialized);
        dataflow::checkpoint chkpt(iterator::getChkpt(), terminator);
        return chkpt;
        //return new dataflow::checkpoint::checkpoint(iterator::getChkpt(), terminator);
}

// Loads this iterator's state from the given checkpoint.
void dataflow::restartFromChkpt(dataflow::checkpoint& chkpt)
{
        iterator::restartFromChkpt(chkpt.iChkpt);
        terminator = chkpt.terminator;
}

string dataflow::str(string indent)
{
        ostringstream outs;
        
        if(initialized) {
                outs << "[dataflow:\n";
                outs << "    iterator = "<<iterator::str(indent+"    ")<<"\n";
                outs << "    terminator = "<< terminator.getNode()->class_name()<<" | "<<terminator.getNode()->unparseToString()<<" | "<<terminator.getIndex()<<"]";
        } else {
                outs << "[dataflow: Uninitialized]";
        }
                
        return outs.str();
}


/*****************************
******* BACK_DATAFLOW *******
*****************************/
back_dataflow::back_dataflow(const DataflowNode &end, const DataflowNode &terminator_arg): 
                iterator(end), dataflow(end, terminator_arg)/*, back_iterator(end)*/ {
}

void back_dataflow::operator ++ (int)
{
        advance(false, true);
}

}
