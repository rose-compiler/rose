# needed to properly define ROSE_UTIL_API
add_definitions(-DROSE_UTIL_EXPORTS)

add_library(util_sawyer OBJECT
  Assert.C Attribute.C Clexer.C CommandLine.C CommandLineBoost.C DocumentBaseMarkup.C RoseDatabase.C DocumentMarkup.C
  DocumentPodMarkup.C DocumentTextMarkup.C GraphTraversal.C LineVector.C
  Message.C ProgressBar.C Sawyer.C SmallObject.C Stopwatch.C Synchronization.C Time.C Yaml.C)

if(SQLITE3_FOUND_LIB)
  target_link_libraries(util_sawyer INTERFACE ${SQLITE3_LIBRARY})
endif()

install(FILES
  Access.h AddressMap.h AddressSegment.h AllocatingBuffer.h Assert.h Attribute.h BiMap.h BitFlags.h BitVector.h BitVectorSupport.h Buffer.h
  Cached.h Callbacks.h Clexer.h CommandLine.h CommandLineBoost.h Database.h DatabasePostgresql.h DatabaseSqlite.h
  DefaultAllocator.h DenseIntegerSet.h DistinctList.h DocumentBaseMarkup.h DocumentMarkup.h DocumentPodMarkup.h
  DocumentTextMarkup.h Exception.h FileSystem.h Graph.h GraphAlgorithm.h GraphBoost.h GraphIteratorBiMap.h GraphIteratorMap.h
  GraphIteratorSet.h GraphTraversal.h IndexedList.h Interval.h IntervalMap.h IntervalSet.h IntervalSetMap.h HashMap.h Lexer.h
  LineVector.h Map.h MappedBuffer.h Message.h NullBuffer.h Optional.h Parse.h PoolAllocator.h ProgressBar.h Sawyer.h Set.h SharedObject.h
  Result.h SharedPointer.h SmallObject.h Stack.h StackAllocator.h StaticBuffer.h Stopwatch.h Synchronization.h ThreadWorkers.h Time.h
  Trace.h Tracker.h Tree.h Type.h WarningsOff.h WarningsRestore.h WorkList.h Yaml.h
  DESTINATION include/rose/Sawyer)
