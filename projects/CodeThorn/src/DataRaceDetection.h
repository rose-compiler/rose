#ifndef DATA_RACE_DETECTION_H
#define DATA_RACE_DETECTION_H

#include <string>
#include "Analyzer.h"
#include "CommandLineOptions.h"

#include "Labeler.h"
#include "VariableIdMapping.h"
#include "Specialization.h"

/*! \brief Data race detection based on a computed state transition
    system and read/write sets.

  * \author Markus Schordan
  * \date 2015, 2016.

  \details This class provides functions for handling command line options
  and a function 'run' that invokes all analyzer functions necessary to perform a
  read-write-set based data race detection.
 */

class SgNode;
class SgPragmaDeclaration;

class DataRaceDetection {
 public:
  DataRaceDetection();
  void setCsvFileName(std::string fileName);
  void handleCommandLineOptions(CodeThorn::Analyzer& analyzer);
  bool run(CodeThorn::Analyzer& analyzer);

  void setCheckAllLoops(bool val);
  void setCheckAllDataRaces(bool val);

  typedef map<SgForStatement*,SgPragmaDeclaration*> ForStmtToOmpPragmaMap;
  static bool isInsideOmpParallelFor(SgNode* node, DataRaceDetection::ForStmtToOmpPragmaMap& forStmtToPragmaMap);
  static bool isOmpParallelFor(SgForStatement* forNode, DataRaceDetection::ForStmtToOmpPragmaMap& forStmtToPragmaMap);
  static LoopInfoSet determineLoopInfoSet(SgNode* root, VariableIdMapping* variableIdMapping, Labeler* labeler);
  static ForStmtToOmpPragmaMap createOmpPragmaForStmtMap(SgNode* root);
  static list<SgPragmaDeclaration*> findPragmaDeclarations(SgNode* root, string pragmaKeyWord);
  static int numParLoops(LoopInfoSet& loopInfoSet, VariableIdMapping* variableIdMapping);

  // computes number of race conditions in update sequence (0:OK, >0:race conditions exist).
  int checkDataRaces(LoopInfoSet& loopInfoSet, ArrayUpdatesSequence& arrayUpdates, VariableIdMapping* variableIdMapping);
  // data race detection
  void populateReadWriteDataIndex(LoopInfo& li, IndexToReadWriteDataMap& indexToReadWriteDataMap, ArrayUpdatesSequence& arrayUpdates,
				  VariableIdMapping* variableIdMapping);
  IndexVector extractIndexVector(LoopInfo& li, const PState* pstate);
  void addAccessesFromExpressionToIndex(SgExpression* exp, IndexVector& index, IndexToReadWriteDataMap& indexToReadWriteDataMap,
					VariableIdMapping* variableIdMapping);
  void displayReadWriteDataIndex(IndexToReadWriteDataMap& indexToReadWriteDataMap, VariableIdMapping* variableIdMapping);
  typedef vector<IndexVector> ThreadVector;
  typedef map<IndexVector,ThreadVector > CheckMapType;
  int numberOfRacyThreadPairs(IndexToReadWriteDataMap& indexToReadWriteDataMap, VariableIdMapping* variableIdMapping);
  void populateCheckMap(CheckMapType& checkMap, IndexToReadWriteDataMap& indexToReadWriteDataMap);
  std::string indexVectorToString(IndexVector iv);
  std::string arrayElementAccessDataSetToString(ArrayElementAccessDataSet& ds, VariableIdMapping* vim);
  void setVisualizeReadWriteAccesses(bool val);

  template <typename T>
    bool dataRaceExistsInvolving1And2(T& wset1, T& rset1, T& wset2, T& rset2, T& writeWriteRaces, T& readWriteRaces);

 private:
  static Sawyer::Message::Facility logger;
  // for data race check of all loops independent on whether they are marked as parallel loops
  bool _checkAllLoops=false;
  bool _checkAllDataRaces=true;
  bool _visualizeReadWriteAccesses=false;
  struct Options {
    Options();
    bool active;
    // fail if data race is detected (terminate, return, and report data race found)
    bool dataRaceFail;
    // name of csv file for data race analysis results. If string is empty, no information is written to file.
    string dataRaceCsvFileName;
    unsigned int maxFloatingPointOperations;
    bool useConstSubstitutionRule;
    bool visualizeReadWriteSets;
    bool printUpdateInfos;
  };
  Options options;
};

// ----- template implementation -----
// Data race definition: 
// Two accesses to the same shared memory location by two different threads, one of which is a write
template <typename T>
bool DataRaceDetection::dataRaceExistsInvolving1And2(T& wset1, T& rset1, T& wset2, T& rset2, 
						  T& writeWriteRaces, T& readWriteRaces) {
  T intersection = wset1 * wset2;
  if(!intersection.empty()) {
    writeWriteRaces.insert(intersection.begin(), intersection.end());
    return true;
  }
  intersection = wset1 * rset2;
  if(!intersection.empty()) {
    readWriteRaces.insert(intersection.begin(), intersection.end());
    return true;
  }
  intersection = rset1 * wset2;
  if(!intersection.empty()) {
    readWriteRaces.insert(intersection.begin(), intersection.end());
    return true;
  }
  return false;
}

#endif
