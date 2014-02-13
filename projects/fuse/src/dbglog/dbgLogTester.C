#include "dbglog.h"
#include "widgets.h"
#include <map>
#include <assert.h>
using namespace std;
using namespace dbglog;

int fibScope(int a, scope::scopeLevel level, int verbosityLevel);
int fibScopeLinks(int a, scope::scopeLevel level, int verbosityLevel, list<int>& stack, 
                  map<list<int>, anchor>& InFW, 
                  map<list<int>, anchor>& InBW, 
                  map<list<int>, anchor>& OutFW, 
                  map<list<int>, anchor>& OutBW,
                  bool doFWLinks);
int fibIndent(int a, int verbosityLevel);

int main(int argc, char** argv)
{
  // It is possible to write arbitrary text to the debug output
  dbg << "Welcome to the dbgLogTester" << endl;
  
  // The emitted text can include arbitrary HTML.
  // Note that erroneous HTML, such as the use of '<' and '>' can produce erroneous debug output
  // and possibly hide messages.
  for(int i=1; i<=4; i++)
    dbg << "<h"<<i<<">This text is an H"<<i<<" header</h"<<i<<">";
  
  // Text can be indented as much as needed
  {
    indent indA;
    dbg << "This text was indented."<<endl;
  }
  dbg << "No indentation here"<<endl;
  
  // Any text can be used for indentation
  indent indB("###");
  dbg << "All subsequent text will have hashes prepended"<<endl;
  dbg << "Such as here\n";
  dbg << "And here"<<endl;
  
  // Here we see recursive function calls (recursive Fibonacci) that add more indentation at deeper levels of recursion
  {
    scope regFibIndent("Indentation due to recursive calls to fib");
    fibIndent(3, 0);
  }
  
  {
    scope regFibIndent("Indentation due to recursive calls to fib, 1 level of indent");
    fibIndent(3, 3);
  }
  
  // In addition to structuring output via indentation it is possible to organize it via visually distinct scopes

  // This creates a high-level scope that lasts only during the lexical scope of object regHigh.
  // A high-level scope creates a separate file and links that make it possible to load the file's
  // context into the parent HTML file or to open the file in a new tab or window.
  {
    scope regHigh("We've entered a top-level scope", scope::high);
    
    // This is a medium-level scope (default) that occurs inside the high-level scope. It also lasts until
    // the end of the scope of object regMed. This object's label is generated using the << syntax, which
    // is used by placing a txt object at the left of all the <<'s.
    scope regMid(txt("This is") << " a lower " << "level scope");
    
    // Write some text into this mid-level scope
    for(int i=0; i<5; i++)
      dbg << "i="<<i<<endl;
  }
  
  // Call the fibScope function, which generates a hierarchy of mid-level scopes, on
  {
    scope regFibIndent("Nested scopes due to recursive calls to fib");
    dbg << "<u>Medium level scopes, colors change</u>"<<endl;
    fibScope(4, scope::medium, 0);
  }
  
   // Call the fib function, which generates a single low-level scope for a=5 and 6
  {
    scope regFibIndent("Nested scopes due to recursive calls to fib, 2 level of scope hierarchy");
    dbg << "<u>Low level scopes, colors do not change</u>"<<endl;
    fibScope(6, scope::low, 5);
  }
  
  // Call the fibScopeLinks function, which generates two hierarchies of high-level scopes with scopes
  // in each level of one hierarchy linking to the same level in the other hierarchy
  {
    scope regFibIndent("Nested scopes due to recursive calls to fib");
    dbg << "<u>High level scopes, colors change and each scope in a new file</u>"<<endl;
    dbg << "There are two hierarchy nests. Sub-scopes at each level of one hierarchy link to the corresponding sub-scopes in the other. Clicking on these links will load up the corresponding scope and its parent scopes."<<endl;
    list<int> stack;
    map<list<int>, anchor> InFW, InBW, OutFW, OutBW;
    fibScopeLinks(5, scope::high, 0, stack, InFW, InBW, OutFW, OutBW, true);
    assert(stack.size()==0);
    map<list<int>, anchor> OutBW2, OutFW2;
    fibScopeLinks(5, scope::high, 0, stack, OutFW, OutBW, OutBW2, OutFW2, false);
  }
  
  dbg << "<h1>It is also possible to generate dot graphs that describe relevant aspects of the code state. Look at dbgLogGraphTester.C</h1>"<<endl;
  
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
  
  /*dbg << "doFWLinks="<<doFWLinks<<", stack=&lt;";
  for(list<int>::iterator i=stack.begin(); i!=stack.end(); i++) {
    if(i!=stack.begin()) dbg << ", ";
    dbg << *i;
  }
  dbg << "&gt; inFW="<<(InFW.find(stack)!=InFW.end())<< endl;*/
  
  // Each recursive call to fibScopeLinks generates a new scope at the desired level. To reduce the amount of text printed, we only 
  // generate scopes if the value of a is >= verbosityLevel
  scope reg(txt()<<"fib("<<a<<")", 
            (InFW.find(stack)!=InFW.end()? InFW[stack]: anchor::noAnchor),
            level, a, verbosityLevel);
  
  OutBW[stack] = reg.getAnchor();
  
  if(a==0 || a==1) { 
    dbg << "=1."<<endl;
    if(doFWLinks) {
      anchor fwLink;
      dbg << fwLink.link("Forward link"); dbg << endl;
      OutFW[stack] = fwLink;
    }
    if(InBW.find(stack)!=InBW.end())
    { dbg << InBW[stack].link("Backward link"); dbg<<endl; }
    
    //cout << "link="<<dbg.linkTo(linkScopes[stack], "go")<<endl;
    stack.pop_back(); // Remove this call from stack
    return 1;
  } else {
    int val = fibScopeLinks(a-1, level, verbosityLevel, stack, InFW, InBW, OutFW, OutBW, doFWLinks) + 
              fibScopeLinks(a-2, level, verbosityLevel, stack, InFW, InBW, OutFW, OutBW, doFWLinks);
    dbg << "="<<val<<endl;
    
    if(doFWLinks) {
      anchor fwLink;
      dbg << fwLink.link("Forward link"); dbg << endl;
      OutFW[stack] = fwLink;
    }
    if(InBW.find(stack)!=InBW.end())
    { dbg << InBW[stack].link("Backward link"); dbg<<endl; }
    
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

