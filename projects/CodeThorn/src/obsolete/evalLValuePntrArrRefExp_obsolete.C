// dead code from here on
if(SgVarRefExp* varRefExp=isSgVarRefExp(arrExp)) {
  PState pstate2=oldPState;
  VariableId arrayVarId=_variableIdMapping->variableId(varRefExp);
  AbstractValue arrayPtrValue;
  // two cases
  if(_variableIdMapping->hasArrayType(arrayVarId)) {
    // create array element 0 (in preparation to have index added, or, if not index is used, it is already the correct index (=0).
    arrayPtrValue=AbstractValue::createAddressOfArray(arrayVarId);
  } else if(_variableIdMapping->hasPointerType(arrayVarId)) {
    // in case it is a pointer retrieve pointer value
    AbstractValue ptr=AbstractValue::createAddressOfArray(arrayVarId);
    if(pstate2.varExists(ptr)) {
      SAWYER_MESG(logger[DEBUG])<<"pointer exists (OK): "<<ptr.toString(_variableIdMapping)<<endl;
      arrayPtrValue=readFromMemoryLocation(estate.label(),&pstate2,ptr);
      SAWYER_MESG(logger[DEBUG])<<"arrayPtrValue: "<<arrayPtrValue.toString(_variableIdMapping)<<endl;
      // convert integer to VariableId
      if(arrayPtrValue.isTop()||arrayPtrValue.isBot()) {
        if(getOptionOutputWarnings())
          cout <<"Warning: array index is top or bot:"<<SgNodeHelper::sourceLineColumnToString(node)<<": "<<node->unparseToString()<<arrayPtrValue.toString(_variableIdMapping)<<"."<<endl;
      }
      // SAWYER_MESG(logger[DEBUG])<<"defering pointer-to-array: ptr:"<<_variableIdMapping->variableName(arrayVarId);
    } else {
      SAWYER_MESG(logger[WARN])<<"Warning: lhs array access: pointer variable does not exist in PState:"<<ptr.toString()<<endl;
      arrayPtrValue=AbstractValue::createTop();
    }
  } else {
    logger[ERROR] <<"lhs array access: unknown type of array or pointer."<<endl;
    exit(1);
  }
  AbstractValue arrayElementAddress;
  //AbstractValue aValue=(*i).value();
  list<SingleEvalResultConstInt> resIntermediate=evaluateExpression(indexExp,estate);
  ROSE_ASSERT(resIntermediate.size()==1); // TODO: temporary restriction
  AbstractValue indexValue=(*(resIntermediate.begin())).value();
  AbstractValue arrayPtrPlusIndexValue=AbstractValue::operatorAdd(arrayPtrValue,indexValue);
  SAWYER_MESG(logger[TRACE])<<"arrayPtrPlusIndexValue: "<<arrayPtrPlusIndexValue.toString(_variableIdMapping)<<endl;
      
  // TODO: rewrite to use AbstractValue only
  {
    //if(arrayPtrPlusIndexValue.isTop()) {
    //    if(_analyzer->getAbstractionMode()!=3) recordPotentialOutOfBoundsAccessLocation(estate.label());
    //} else {
    //VariableId arrayVarId2=arrayPtrPlusIndexValue.getVariableId();
    //int index2=arrayPtrPlusIndexValue.getIndexIntValue();
    //if(!accessIsWithinArrayBounds(arrayVarId2,index2)) {
    //if(_analyzer->getAbstractionMode()!=3) recordDefinitiveOutOfBoundsAccessLocation(estate.label());
    //cerr<<"Program error detected at "<<SgNodeHelper::sourceLineColumnToString(node)<<" : write access out of bounds."<<endl;
    //}
    //}
  }

  arrayElementAddress=arrayPtrPlusIndexValue;
  //cout<<"DEBUG: arrayElementAddress: "<<arrayElementAddress.toString(_variableIdMapping)<<endl;
  //SAWYER_MESG(logger[TRACE])<<"arrayElementVarId:"<<arrayElementAddress.toString()<<":"<<_variableIdMapping->variableName(arrayVarId)<<" Index:"<<index<<endl;
  ROSE_ASSERT(!arrayElementAddress.isBot());
  // read value of variable var id (same as for VarRefExp - TODO: reuse)
  // TODO: check whether arrayElementAddress (or array) is a constant array (arrayVarId)
  //if(!pstate2.varExists(arrayElementAddress)) {
  // check that array is constant array (it is therefore ok that it is not in the state)
  //SAWYER_MESG(logger[TRACE]) <<"lhs array-access index does not exist in state (creating it as address now). Array element id:"<<arrayElementAddress.toString(_variableIdMapping)<<" PState size:"<<pstate2.size()<<endl;
  //}
  res.result=arrayElementAddress;
  return listify(res);
 } else {
  logger[ERROR] <<"array-access uses expr for denoting the array. Normalization missing."<<endl;
  logger[ERROR] <<"expr: "<<node->unparseToString()<<endl;
  logger[ERROR] <<"arraySkip: "<<getSkipArrayAccesses()<<endl;
  exit(1);
 }
