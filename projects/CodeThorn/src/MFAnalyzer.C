/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#ifndef MFANALYZER_C
#define MFANALYZER_C

template<typename LatticeType>
void MFAnalyzer<LatticeType>::setExtremalLabels(set<Label> extremalLabels) {
  _extremalLabels=extremalLabels;
}
template<typename LatticeType>
MFAnalyzer<LatticeType>::MFAnalyzer(CFAnalyzer* cfanalyzer):_labeler(0),_cfanalyzer(cfanalyzer),_numberOfLabels(0) {
  assert(cfanalyzer);
  _labeler=cfanalyzer->getLabeler();
  _numberOfLabels=_labeler->numberOfLabels();
}

template<typename LatticeType>
void
MFAnalyzer<LatticeType>::initialize() {
  for(long l=0;l<_numberOfLabels;++l) {
	LatticeType le;
	_analyzerData.push_back(le);
  }
  cout << "STATUS: initialized monotone data flow analysis framework for "<<_analyzerData.size()<< " labels."<<endl;
}

template<typename LatticeType>
void
MFAnalyzer<LatticeType>::run() {
  // runs until worklist is empty
  for(long i=0;i<_numberOfLabels;++i) {
	_analyzerData[i]=transfer(_analyzerData[i]);
  }
}

// default identity function
template<typename LatticeType>
LatticeType
MFAnalyzer<LatticeType>::transfer(Edge edge, LatticeType element) {
  return element;
}
#endif
