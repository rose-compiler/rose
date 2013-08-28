#include "dbglog.h"
#include "widgets.h"
#include <map>
#include <assert.h>
using namespace std;
using namespace dbglog;

class dottableExample: public dottable
{
  public:
  // Returns a string that containts the representation of the object as a graph in the DOT language
  // that has the given name
  std::string toDOT(std::string graphName) {
    ostringstream oss;
    oss << "graph ethane {"<<endl;
    oss << "     C_0 -- H_0 [type=s];"<<endl;
    oss << "     C_0 -- H_1 [type=s];"<<endl;
    oss << "     C_0 -- H_2 [type=s];"<<endl;
    oss << "     C_0 -- C_1 [type=s];"<<endl;
    oss << "     C_1 -- H_3 [type=s];"<<endl;
    oss << "     C_1 -- H_4 [type=s];"<<endl;
    oss << "     C_1 -- H_5 [type=s];"<<endl;
    oss << " }";
    return oss.str();
  }
};

int main(int argc, char** argv)
{
  // It is possible to write arbitrary text to the debug output
  dbg << "Welcome to the dbgLogLogTester" << endl;
  
  // Dot graph
  dbg << "It is possible to generate dot graphs that describe relevant aspects of the code state.";
  
  {
    scope dotStr("This graph was generated from a string:", scope::medium);
    //string imgPath = addDOT(string("graph graphname {\n     a -- b -- c;\n     b -- d;\n}"));
    //dbg << "imgPath=\""<<imgPath<<"\"\n";
    graph::genGraph(string("graph graphname {\n     a -- b -- c;\n     b -- d;\n}"));
  }
  
  {
    scope dotStr("This graph was generated from a dottable object:", scope::medium);
    dottableExample ex;
    /*string imgPath = addDOT(ex);
    dbg << "imgPath=\""<<imgPath<<"\"\n";*/
    graph::genGraph(ex);
  }
  
  {
    scope s("Graph1: link each number to its multiples", scope::high);
    graph g;
    
    map<int, set<anchor> > pointsTo;
    int maxVal=20;
    for(int i=2; i<maxVal; i++) {
      scope s(txt()<<"s"<<i, pointsTo[i], scope::medium);
      for(int j=i*2; j<maxVal; j+=i) {
        {
        anchor toAnchor;
        //cout << "    toAnchor="<<toAnchor.str()<<endl;
        pointsTo[j].insert(toAnchor);
        g.addDirEdge(s.getAnchor(), toAnchor);
        //cout << "    toAnchor="<<toAnchor.str()<<endl;
      }
      }
    }
  }
  
  {
    int x=5;
    scope s(txt()<<"Graph2: link all numbers (mod "<<x<<") together in a ring", scope::high);
    graph g;
    
    map<int, anchor> ringStartA;
    map<int, anchor> ringNextA;
    int maxVal=20;
    for(int i=0; i<maxVal; i++) {
      scope s(txt()<<"s"<<i, 
              // Scopes that are first in their ring have no incoming link yet (we'll create one when we reach the last scope in the ring)
              i/x==0 ? anchor::noAnchor: 
              // Other scopes are linked to from their predecessor. In particular, their predecessor created the link to this scope
              // before the scope was formed, we terminate it here to complete the connection
                       ringNextA[i%x],
              scope::medium);
      
      // If this is the first scope in its ring
      if(i/x==0) {
        // Record its anchor
        ringStartA[i%x] = s.getAnchor();
      }
      
      // If the next scope in the ring is not its first
      if(i+x < maxVal) {
        // Add a forward link to the next scope in the ring and record the target anchor of the link
        // so that we can identify the scope where it terminates when we reach that scope.
        anchor toAnchor;
        g.addDirEdge(s.getAnchor(), toAnchor);
        ringNextA[i%x] = toAnchor;
      } else {
        // Otherwise, add a backward link to the starting scope of the ring
        g.addDirEdge(s.getAnchor(), ringStartA[i%x]);
      }
    }
  }
    
  return 0;
}

int fibScope(int a, scope::scopeLevel level, int verbosityLevel) {
  // Each recursive call to fibScope() generates a new scope at the desired level. To reduce the amount of text printed, we only 
  // generate scopes if the value of a is >= verbosityLevel
  scope reg(txt()<<"fib("<<a<<")", level, a, verbosityLevel);
  
  if(a==0 || a==1) { 
    dbg << "=1."<<endl;
    return 1;
  } else {
    int val = fibScope(a-1, level, verbosityLevel) + 
              fibScope(a-2, level, verbosityLevel);
    dbg << "="<<val<<endl;
    return val;
  }
}

// InFW links: links from prior fib call to this one. These need to be anchored to the regions in this next.
// InBW links: links from this fib call nest to the prior one. These have already been anchored to the prior nest.
// OutBW links: links from the next nest to this one. These are anchored to established regions.
// OutFW links: links from this nest to the next one. These are un-anchored and will be anchored to the next nest
int fibScopeLinks(int a, scope::scopeLevel level, int verbosityLevel, list<int>& stack, 
                  map<list<int>, anchor>& InFW, 
                  map<list<int>, anchor>& InBW, 
                  map<list<int>, anchor>& OutFW,
                  map<list<int>, anchor>& OutBW,
                  bool doFWLinks) {
  stack.push_back(a); // Add this call to stack
  
  /*dbg << "stack=&lt;";
  for(list<int>::iterator i=stack.begin(); i!=stack.end(); i++) {
    if(i!=stack.begin()) dbg << ", ";
    dbg << *i;
  }
  dbg << "&gt; inFW="<<(InFW.find(stack)!=InFW.end())<< endl;*/
  
  // Each recursive call to fibScopeLinks generates a new scope at the desired level. To reduce the amount of text printed, we only 
  // generate scopes if the value of a is >= verbosityLevel
  scope reg(txt()<<"fib("<<a<<")", 
            (InFW.find(stack)!=InFW.end()? &InFW[stack]: &anchor::noAnchor),
            level, a, verbosityLevel);
  
  OutBW[stack] = reg.getAnchor();
  
  if(a==0 || a==1) { 
    dbg << "=1."<<endl;
    if(doFWLinks) {
      anchor fwLink;
      fwLink.link("Forward link"); dbg << endl;
      OutFW[stack] = fwLink;
    }
    if(InBW.find(stack)!=InBW.end())
    { InBW[stack].link("Backward link"); dbg<<endl; }
    
    //cout << "link="<<dbg.linkTo(linkScopes[stack], "go")<<endl;
    stack.pop_back(); // Remove this call from stack
    return 1;
  } else {
    int val = fibScopeLinks(a-1, level, verbosityLevel, stack, InFW, InBW, OutFW, OutBW, doFWLinks) + 
              fibScopeLinks(a-2, level, verbosityLevel, stack, InFW, InBW, OutFW, OutBW, doFWLinks);
    dbg << "="<<val<<endl;
    
    if(doFWLinks) {
      anchor fwLink;
      fwLink.link("Forward link"); dbg << endl;
      OutFW[stack] = fwLink;
    }
    if(InBW.find(stack)!=InBW.end())
    { InBW[stack].link("Backward link"); dbg<<endl; }
    
    //cout << "link="<<dbg.linkTo(linkScopes[stack], "go")<<endl;
    stack.pop_back(); // Remove this call from stack
    return val;
  }
}

int fibIndent(int a, int verbosityLevel) {
  // Each recursive call to fibScopeLinks adds an indent level, prepending ":" to text printed by deeper calls to fibIndent. 
  // To reduce the amount of text printed, we only add indentation if the value of a is >= verbosityLevel
  indent ind(": ", a, verbosityLevel);
  
  if(a==0 || a==1) { 
    dbg << "=1"<<endl;
    return 1;
  } else {
    int val = fibIndent(a-1, verbosityLevel) + fibIndent(a-2, verbosityLevel);
    dbg << "="<<val<<endl;
    return val;
  }
}

