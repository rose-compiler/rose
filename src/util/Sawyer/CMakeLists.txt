add_library(util_sawyer OBJECT
  Assert.C CommandLine.C GraphTraversal.C Markup.C MarkupPod.C
  Message.C ProgressBar.C Sawyer.C SmallObject.C Stopwatch.C Synchronization.C)

install(FILES
    Access.h AddressMap.h AddressSegment.h AllocatingBuffer.h Assert.h
    BiMap.h BitVector.h BitVectorSupport.h Buffer.h Cached.h
    Callbacks.h CommandLine.h DefaultAllocator.h DistinctList.h
    Graph.h GraphAlgorithm.h GraphBoost.h GraphTraversal.h
    IndexedList.h Interval.h IntervalMap.h IntervalSet.h
    IntervalSetMap.h Map.h MappedBuffer.h Markup.h MarkupPod.h
    Message.h NullBuffer.h Optional.h PoolAllocator.h ProgressBar.h
    Sawyer.h Set.h SharedPointer.h SmallObject.h Stack.h
    StaticBuffer.h Stopwatch.h Synchronization.h ThreadWorkers.h
    WarningsOff.h WarningsRestore.h
  DESTINATION ${INCLUDE_INSTALL_DIR}/Sawyer)

