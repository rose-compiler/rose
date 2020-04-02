#include "sage3basic.h"
#include "IOSequenceGenerator.h"

using namespace std;

namespace CodeThorn {
  InputPath::InputPath():_numInputStates(0) {
  }
  int InputPath::addState(const EState* s) {
    if(s->io.isStdInIO()) {
      _numInputStates++;
    }
    if(s->io.isStdInIO()||s->io.isFailedAssertIO()) {
      push_back(s);
      return 1;
    } else {
      return 0;
    }
  }
  string InputPath::toString() const {
    stringstream ss;
    for(InputPath::const_iterator i=this->begin();i!=this->end();++i) {
      string sep=" ";
      if(i!=this->begin()) {
        ss<<sep;
      }
      if((*i)->io.isStdInIO()) {
        AbstractValue val=(*i)->determineUniqueIOValue();
        ss<<val.toString();
      } else {
        ss<<"ERROR:"<<(*i)->toString();
      }
    }
    return ss.str();
  }
  unsigned int InputPath::numInputStates() const { return _numInputStates; }

  void InputPathSet::toFile(ofstream& os) {
    for(InputPathSet::iterator i=this->begin();i!=this->end();++i) {
      os<<(*i).toString()<<endl;
    }
  }
 
  IOSequenceGenerator::IOSequenceGenerator():randomSearch(false) {
  }

  void IOSequenceGenerator::computeInputPathSet(unsigned int maxlen, TransitionGraph& g) {
    InputPath p;
    const EState* s=g.getStartEState();
    randomSearch=false;
    computeInputPathSet(p,0,maxlen,s,g,ipset);
  }
  void IOSequenceGenerator::computeRandomInputPathSet(unsigned int maxlen, TransitionGraph& g, int randomNum) {
    computeInputPathSet(maxlen,g);
    InputPathSet ipset2;
    if(ipset.size()>0) {
      for(int i=0;i<randomNum;++i) {
        InputPath randomElement=this->chooseElement(ipset);
        ipset2.insert(randomElement);
      }
      ipset=ipset2;
    }
  }
  void IOSequenceGenerator::generateFile(string fileName) {
    ofstream myfile;
    myfile.open(fileName.c_str());
    ipset.toFile(myfile);
    myfile.close();
  }

  const EState* IOSequenceGenerator::chooseElement(EStatePtrSet& S0) {
    if(S0.size()>0) {
      EStatePtrSet::const_iterator it(S0.begin());
      int r = rand() % S0.size();
      std::advance(it,r);
      return *it;
    } else {
      return 0;
    }
  }

  InputPath IOSequenceGenerator::chooseElement(InputPathSet& S0) {
    if(S0.size()>0) {
      InputPathSet::const_iterator it(S0.begin());
      int r = rand() % S0.size();
      std::advance(it,r);
      return *it;
    } else {
      cerr<<"Error: choosing from empty set."<<endl;
      exit(1);
    }
  }

  void IOSequenceGenerator::computeInputPathSet(InputPath p, int len, unsigned int maxlen, const EState* s, TransitionGraph& g, InputPathSet& pathSet) {
    // add one additional state to ensure found state is not followed by failing assert
    if(p.numInputStates()<maxlen+1) {
      EStatePtrSet S;
      if(randomSearch) {
        EStatePtrSet S0=g.succ(s);
        const EState* randomElement=this->chooseElement(S0);
        if(randomElement!=0) {
          S.insert(randomElement);
        }
      } else {
        S=g.succ(s);
      }
      for(EStatePtrSet::iterator i=S.begin();i!=S.end();++i) {
        const EState* succState=*i;
        int num=0;
        InputPath p2=p;
        if(succState->io.isStdInIO()||succState->io.isFailedAssertIO()) {
          num=p2.addState(*i);
        }
        // only continue on this path if it is not a failing assertion OR output on stderr (RERS specific)
        if(!(succState->io.isFailedAssertIO()||succState->io.isStdErrIO())) {
          computeInputPathSet(p2,len+num,maxlen,succState,g,pathSet);
        }
      }
    } else {
      // only insert if last element if it is not a failing assertion OR output on stderr (RERS specific)
      if(p.numInputStates()>0) {
        const EState* lastState=p.back();
        if(!(lastState->io.isFailedAssertIO()||lastState->io.isStdErrIO())) {
          // remove last state
          p.pop_back();
          pathSet.insert(p);
        }
      }
    }
  }
} // namespace CodeThorn
