add_library(util_support OBJECT
  BitVectorRepr.C CommandOptions.C VectorCommandOptions.C)

########### install files ###############
install(FILES
  DAG.h IteratorCompound.h TreeImpl.h BitVectorRepr.h DirectedGraph.h
  IteratorTmpl.h PtrMap.h union_find.h VectorCommandOptions.h CommandOptions.h
  DoublyLinkedList.h LatticeElemList.h PtrSet.h const.h FunctionObject.h
  Matrix.h SinglyLinkedList.h CountRefHandle.h general.h ObserveObject.h
  StreamIO.h FileHelper.h CollectionHelper.h
  DESTINATION ${INCLUDE_INSTALL_DIR})
