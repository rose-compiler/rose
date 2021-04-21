int aux() {
  list<SingleEvalResultConstInt> evalResultListF=exprAnalyzer.evaluateExpression(nextNodeToAnalyze2,currentEState,false);
  list<SingleEvalResultConstInt> evalResultList=exprAnalyzer.evaluateExpression(nextNodeToAnalyze2,currentEState,true);
  //  if(evalResultListF.size()!=evalResultList.size()) {
  //cout<<"DEBUG: different evalresultList sizes (false vs true):"<<evalResultList.size()<<":"<<evalResultListF.size()<<endl;
  for(list<SingleEvalResultConstInt>::iterator i=evalResultList.begin();
      i!=evalResultList.end();
      ++i) {
    SingleEvalResultConstInt evalResult=*i;
    if(evalResult.isBot()) cout <<" bot";
    if(evalResult.isTop()) cout <<" top";
    if(evalResult.isTrue()) cout <<" true";
    if(evalResult.isFalse()) cout <<" false";
  }
  cout <<" vs ";
  for(list<SingleEvalResultConstInt>::iterator i=evalResultListF.begin();
      i!=evalResultListF.end();
      ++i) {
    SingleEvalResultConstInt evalResult=*i;
    if(evalResult.isBot()) cout <<" bot";
    if(evalResult.isTop()) cout <<" top";
    if(evalResult.isTrue()) cout <<" true";
    if(evalResult.isFalse()) cout <<" false";
  }
  cout<<" @ "<<nextNodeToAnalyze2->unparseToString();
  cout<<endl;
}
