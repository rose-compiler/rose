#ifndef IOSequenceGenerator_H
#define IOSequenceGenerator_H

#include "EState.h"
#include <iostream>
#include <sstream>

using namespace std;

namespace CodeThorn {
  // predefined predicate
  class InputPath : public list<const EState*> {
  public:
    // adds a state to the input path if it is an input state (returns 1 in this case, otherwise 0 (for counting)).
    InputPath():_numInputStates(0){}
    int addState(const EState* s) {
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
    string toString() const {
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
    unsigned int numInputStates() const { return _numInputStates; }
  private:
    unsigned int _numInputStates;
  };

  class InputPathSet : public set<InputPath> {
    // IO Functions to be added
  public:
    void toFile(ofstream& os) {
      for(InputPathSet::iterator i=this->begin();i!=this->end();++i) {
        os<<(*i).toString()<<endl;
      }
    }
  };
  
  class IOSequenceGenerator {
  public:
    IOSequenceGenerator():randomSearch(false) {}
    void computeInputPathSet(unsigned int maxlen, TransitionGraph& g) {
      InputPath p;
      const EState* s=g.getStartEState();
      randomSearch=false;
      computeInputPathSet(p,0,maxlen,s,g,ipset);
    }
    void computeRandomInputPathSet(unsigned int maxlen, TransitionGraph& g, int randomNum) {
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
    void generateFile(string fileName) {
      ofstream myfile;
      myfile.open(fileName.c_str());
      ipset.toFile(myfile);
      myfile.close();
    }
  private:
    InputPathSet ipset;
    bool randomSearch;

    const EState* chooseElement(EStatePtrSet& S0) {
      if(S0.size()>0) {
        EStatePtrSet::const_iterator it(S0.begin());
        int r = rand() % S0.size();
        std::advance(it,r);
        return *it;
      } else {
        return 0;
      }
    }

    InputPath chooseElement(InputPathSet& S0) {
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

    void computeInputPathSet(InputPath p, int len, unsigned int maxlen, const EState* s, TransitionGraph& g, InputPathSet& pathSet) {
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
  };
} // namespace CodeThorn

#endif
