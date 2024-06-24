# [Thomas Gorham 6-14-24] Per Sawyer.h comment around line 337 that reads
# "[Robb Matzke 2014-06-18]: get rid of ROSE_UTIL_EXPORTS;" 
if(WIN32) 
  add_definitions(-DSAWYER_DO_EXPORTS)
endif()

# Boost is compiled with multi-thread support on linux so we must add "-pthread" to all compile and link commands, or
# whatever is appropriate for the system.  With GCC it is not sufficient to just add "-lpthread" to the link
# commands--the "-pthread" switch MUST be added to the compile commands as well.
# Support multi-threaded support to for Sawyer and tools that use sawyer 
if(NOT Boost_THREAD_FOUND AND NOT WIN32)
  message(WARNING "Boost.Thread package was not found. Threading support may be limited")
endif()

if(NOT Threads_FOUND AND NOT WIN32) 
  message(WARNING "Threads package was not found. Threading support may be limited") 
endif()

# MAYBE TODO: Change to static 
add_library(util_sawyer OBJECT
  Assert.C Attribute.C Clexer.C CommandLine.C CommandLineBoost.C DocumentBaseMarkup.C RoseDatabase.C DocumentMarkup.C
  DocumentPodMarkup.C DocumentTextMarkup.C GraphTraversal.C LineVector.C
  Message.C ProgressBar.C Sawyer.C SmallObject.C Stopwatch.C Synchronization.C Time.C Yaml.C)

add_compile_definitions(util_sawyer PRIVATE COMPILING_ROSE) 

target_link_libraries(util_sawyer PRIVATE $<$<BOOL:${Threads_FOUND}>:Threads::Threads> ${Boost_LIBRARIES}) 

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
