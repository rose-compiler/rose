#include "rose.h"
#include "createSignatureVectors.h"
#include "vectorCompression.h"
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <ext/hash_map>

//MD5 sum header files
#include <openssl/md5.h>
#include <stdio.h>
#include <string.h>


using namespace std;
using namespace sqlite3x;
using namespace __gnu_cxx;

namespace __gnu_cxx {
  template <>
  struct hash<SgAsmExpression*> {
    size_t operator()(SgAsmExpression* expr) const {
      return size_t(expr);
    }
  };
  template <>
  struct hash<void*> {
    size_t operator()(void* expr) const {
      return size_t(expr);
    }
  };
}

#define NORMALIZED_UNPARSED_INSTRUCTIONS

enum ExpressionCategory {ec_reg = 0, ec_mem = 1, ec_val = 2};

#if 1
static const size_t numberOfInstructionKinds = x86_last_instruction;
inline size_t getInstructionKind(SgAsmx86Instruction* insn) {return insn->get_kind();}
#else
static const size_t numberOfInstructionKinds = V_SgNumVariants;
inline size_t getInstructionKind(SgAsmx86Instruction* insn) {return insn->variantT();}
#endif

size_t numVectorsGenerated = 0;

static map<string, void*> internTable;

inline void* intern(const std::string& s) {
  map<string, void*>::iterator i = internTable.find(s);
  if (i == internTable.end()) {
    void* sCopy = new string(s);
    internTable.insert(std::make_pair(s, sCopy));
    return sCopy;
  } else {
    return i->second;
  }
}

static hash_map<SgAsmExpression*, void*> unparseAndInternTable;

inline void* unparseAndIntern(SgAsmExpression* e) {
  hash_map<SgAsmExpression*, void*>::const_iterator i = unparseAndInternTable.find(e);
  if (i == unparseAndInternTable.end()) {
    void* sPtr = intern(unparseX86Expression(e, NULL, NULL));
    unparseAndInternTable.insert(std::make_pair(e, sPtr));
    return sPtr;
  } else {
    return i->second;
  }
}
 
class SignatureVector {
  public:
  static const size_t Size = numberOfInstructionKinds * 4 + 300 + 9 + 3;
  typedef uint16_t ElementType;

  private:
  ElementType values[Size];

  public:
  SignatureVector() {
    clear();
  }

  void clear() {
    for (size_t i = 0; i < Size; ++i) values[i] = 0;
  }

  ElementType operator[](size_t i) const {assert (i < Size); return values[i];}

  ElementType& totalForVariant(size_t var) {assert (var < numberOfInstructionKinds); return values[var * 4];}
  ElementType& opsForVariant(ExpressionCategory cat, size_t var) {assert (var < numberOfInstructionKinds); return values[var * 4 + (int)cat + 1];}
  ElementType& specificOp(ExpressionCategory cat, size_t num) {
	static ElementType dummyVariable = 0;
	if (num < 100) 
	  return values[numberOfInstructionKinds * 4 + 100 * (int)cat + num];
	else
	  return dummyVariable;
  }
  ElementType& operandPair(ExpressionCategory a, ExpressionCategory b) {return values[numberOfInstructionKinds * 4 + 300 + (int)a * 3 + (int)b];}
  ElementType& operandTotal(ExpressionCategory a) {return values[numberOfInstructionKinds * 4 + 300 + 9 + (int)a];}

  const ElementType* getBase() const {return values;}
};

inline ExpressionCategory getCategory(SgAsmExpression* e) {
  if (isSgAsmValueExpression(e)) {
    return ec_val;
  } else if (isSgAsmRegisterReferenceExpression(e)) {
    return ec_reg;
  } else if (isSgAsmMemoryReferenceExpression(e)) {
    return ec_mem;
  } else {
    abort();
  }
}

SgAsmExpressionPtrList& getOperands(SgAsmInstruction* insn) {
  SgAsmOperandList* ol = insn->get_operandList();
  SgAsmExpressionPtrList& operands = ol->get_operands();
  return operands;
}

void numberOperands(SgAsmx86Instruction* firstInsn[], size_t insnCount, hash_map<SgAsmExpression*, size_t> numbers[3]) {
  hash_map<void*, size_t> stringNumbers[3];
  for (size_t i = 0; i < insnCount; ++i) {
    SgAsmx86Instruction* insn = firstInsn[i];
    const SgAsmExpressionPtrList& operands = getOperands(insn);
    //size_t operandCount = operands.size();
    for (size_t j = 0; j < operands.size(); ++j) {
      SgAsmExpression* e = operands[j];
      ExpressionCategory cat = getCategory(e);
      void* str = unparseAndIntern(e);
      hash_map<void*, size_t>& currentStringNums = stringNumbers[(int)cat];
      hash_map<void*, size_t>::const_iterator stringNumIter = currentStringNums.find(str);
      size_t num = (stringNumIter == currentStringNums.end() ? currentStringNums.size() : stringNumIter->second);
      if (stringNumIter == currentStringNums.end()) currentStringNums.insert(std::make_pair(str, num));
      numbers[(int)cat][e] = num;
    }
  }
}

void addVectorToDatabase(sqlite3_connection& con, const SignatureVector& vec, const std::string& functionName, size_t functionId, size_t indexWithinFunction, const std::string& normalizedUnparsedInstructions, SgAsmx86Instruction* firstInsn[], const std::string& filename, size_t windowSize, size_t stride);
void addFunctionStatistics(sqlite3_connection& con, const std::string& filename, const std::string& functionName, size_t functionId, size_t numInstructions);

bool createVectorsForAllInstructions(SgNode* top, const std::string& filename, const std::string& functionName, int functionId, size_t windowSize, size_t stride, sqlite3_connection& con) { // Ignores function boundaries
  bool retVal = false;
  vector<SgAsmx86Instruction*> insns;
  FindInstructionsVisitor vis;
  AstQueryNamespace::querySubTree(top, std::bind2nd( vis, &insns ));
  std::cout << "Number of instructions: " << insns.size() << std::endl;
  size_t insnCount = insns.size();

  for (size_t windowStart = 0;
       windowStart + windowSize <= insnCount;
       windowStart += stride) {
    static SignatureVector vec;
    vec.clear();
    hash_map<SgAsmExpression*, size_t> valueNumbers[3];
    numberOperands(&insns[windowStart], windowSize, valueNumbers);
    string normalizedUnparsedInstructions;
    // Unparse the normalized forms of the instructions
    for (size_t insnNumber = 0; insnNumber < windowSize; ++insnNumber) {
      SgAsmx86Instruction* insn = insns[windowStart + insnNumber];
      size_t var = getInstructionKind(insn);
#ifdef NORMALIZED_UNPARSED_INSTRUCTIONS
      string mne = insn->get_mnemonic();
      boost::to_lower(mne);
      normalizedUnparsedInstructions += mne;
#endif
      const SgAsmExpressionPtrList& operands = getOperands(insn);
      size_t operandCount = operands.size();
      // Add to total for this variant
      ++vec.totalForVariant(var);
      // Add to total for each kind of operand
      for (size_t i = 0; i < operandCount; ++i) {
        SgAsmExpression* operand = operands[i];
        ExpressionCategory cat = getCategory(operand);
        ++vec.opsForVariant(cat, var);
        // Add to total for this unique operand number (for this window)
        hash_map<SgAsmExpression*, size_t>::const_iterator numIter = valueNumbers[(int)cat].find(operand);
        assert (numIter != valueNumbers[(int)cat].end());
        size_t num = numIter->second;
        ++vec.specificOp(cat, num);
        // Add to total for this kind of operand
        ++vec.operandTotal(cat);
#ifdef NORMALIZED_UNPARSED_INSTRUCTIONS
        normalizedUnparsedInstructions += (cat == ec_reg ? "R" : cat == ec_mem ? "M" : "V") + boost::lexical_cast<string>(num);
#endif
      }

	  //Try to see what the effect is of jumps on the false positive rate
	  //uint64_t addr =0;
          /*
	  if( x86GetKnownBranchTarget(insn, addr) == true  )
	  {
		uint64_t insn_addr = insn->get_address();
		if( addr < insn_addr )
		  normalizedUnparsedInstructions += " UP ";
		else
		  normalizedUnparsedInstructions += " DOWN ";
	  }*/
	  
      // Add to total for this pair of operand kinds
      if (operandCount >= 2) {
        ExpressionCategory cat1 = getCategory(operands[0]);
        ExpressionCategory cat2 = getCategory(operands[1]);
        ++vec.operandPair(cat1, cat2);
      }
#ifdef NORMALIZED_UNPARSED_INSTRUCTIONS
      if (insnNumber + 1 < windowSize) {
        normalizedUnparsedInstructions += ";";
      }
#endif
    }

#if 0
    // Print out this vector
    cout << "{";
    for (size_t i = 0; i < SignatureVector::Size; ++i) {
      if (i != 0) cout << ", ";
      cout << vec[i];
    }
    cout << "}\n";
#endif

    // cout << "Normalized instruction stream: " << normalizedUnparsedInstructions << endl;

    // Add vector to database
    addVectorToDatabase(con, vec, functionName, functionId, windowStart/stride, normalizedUnparsedInstructions, &insns[windowStart], filename, windowSize, stride);
	retVal = true;
  }
  addFunctionStatistics(con, filename, functionName, functionId, insnCount);
  return retVal;
}


void createVectorsNotRespectingFunctionBoundaries(SgNode* top, const std::string& filename, size_t windowSize, size_t stride, sqlite3_connection& con) {
  //row_numbers start at 0
  int functionId=0;
  try{
	string query = "SELECT max(row_number) from function_ids";

	sqlite3_command cmd2(con, query /* selectQuery.str() */ );

	sqlite3_reader entriesForFile =cmd2.executereader();

	if(entriesForFile.read())
	{
	  functionId = entriesForFile.getint(0);
	}
  } catch(exception &ex) {
	cerr << "Exception Occurred: " << ex.what() << endl;
  }
  
  functionId++;

  std::string functionName = filename+"-all-instructions";
  createVectorsForAllInstructions(top, filename, functionName, functionId,windowSize, stride, con);

  try{
    string query = "INSERT into function_ids(file,function_name) VALUES(?,?) ";

    sqlite3_command cmd(con, query /* selectQuery.str() */ );

    cmd.bind(1, filename);
    cmd.bind(2, functionName );
    cmd.executenonquery();
  } catch(exception &ex) {
    cerr << "Exception Occurred: " << ex.what() << endl;
  }

  cout << "Total vectors generated: " << numVectorsGenerated << endl;
}

void createVectorsRespectingFunctionBoundaries(SgNode* top, const std::string& filename, size_t windowSize, size_t stride, sqlite3_connection& con) {
  vector<SgAsmFunction*> funcs;
  FindAsmFunctionsVisitor vis;
  AstQueryNamespace::querySubTree(top, std::bind2nd( vis, &funcs ));
  size_t funcCount = funcs.size();

  //row_numbers start at 0
  int functionId=0;
  try{
	string query = "SELECT max(row_number) from function_ids";

	sqlite3_command cmd2(con, query /* selectQuery.str() */ );

	sqlite3_reader entriesForFile =cmd2.executereader();

	if(entriesForFile.read())
	{
	  functionId = entriesForFile.getint(0);
	}
  } catch(exception &ex) {
	cerr << "Exception Occurred: " << ex.what() << endl;
  }
  
  functionId++;
  for (size_t i = 0; i < funcCount; ++i) {
	createVectorsForAllInstructions(funcs[i], filename, funcs[i]->get_name(), functionId, windowSize, stride, con);
	// if( addedVectors == true )
	{
		try{
		  string query = "INSERT into function_ids(file,function_name) VALUES(?,?) ";

		  sqlite3_command cmd(con, query /* selectQuery.str() */ );

		  cmd.bind(1, filename);
		  cmd.bind(2, funcs[i]->get_name() );
		  cmd.executenonquery();
		} catch(exception &ex) {
		  cerr << "Exception Occurred: " << ex.what() << endl;
		}
     	functionId++;

	}

	
  }
  cerr << "Total vectors generated: " << numVectorsGenerated << endl;
}

void addFunctionStatistics(sqlite3_connection& con, const std::string& filename, const std::string& functionName, size_t functionId, size_t numInstructions) {
  string add_function_stats = "INSERT INTO function_statistics(function_id, num_instructions) VALUES(?,?)";

  sqlite3_command cmd(con, add_function_stats.c_str());
  cmd.bind(1, (int)functionId);
  cmd.bind(2, (int)numInstructions);

  cmd.executenonquery();
}

void addVectorToDatabase(sqlite3_connection& con, const SignatureVector& vec, const std::string& functionName, size_t functionId, size_t indexWithinFunction, const std::string& normalizedUnparsedInstructions, SgAsmx86Instruction* firstInsn[], const std::string& filename, size_t windowSize, size_t stride) {
  ++numVectorsGenerated;

  vector<uint8_t> compressedCounts = compressVector(vec.getBase(), SignatureVector::Size);
  size_t vectorSum = 0;
  for (size_t i = 0; i < SignatureVector::Size; ++i) {
    vectorSum += vec[i];
  }

  string db_select_n = "INSERT INTO vectors( function_id,  index_within_function, line, offset, sum_of_counts, counts, instr_seq ) VALUES(?,?,?,?,?,?,?)";
  string line = boost::lexical_cast<string>(isSgAsmStatement(firstInsn[0])->get_address());
  string offset = boost::lexical_cast<string>(isSgAsmStatement(firstInsn[windowSize - 1])->get_address());


  unsigned char md[16];
  //calculate_md5_of( (const unsigned char*) normalizedUnparsedInstructions.data() , normalizedUnparsedInstructions.size(), md ) ;
  MD5( (const unsigned char*) normalizedUnparsedInstructions.data() , normalizedUnparsedInstructions.size(), md ) ;

  sqlite3_command cmd(con, db_select_n.c_str());
  cmd.bind(1, (int)functionId );
  cmd.bind(2, (int)indexWithinFunction );
  cmd.bind(3, line);
  cmd.bind(4, offset);
  cmd.bind(5, boost::lexical_cast<string>(vectorSum));
  cmd.bind(6, &compressedCounts[0], compressedCounts.size());
  cmd.bind(7, md,16);
  //cmd.bind(7, "");

  cmd.executenonquery();
}

void createDatabases(sqlite3_connection& con) {
  try {
	  con.executenonquery("create table IF NOT EXISTS run_parameters(window_size INTEGER, stride INTEGER)");
  }
  catch(exception &ex) {
	cerr << "Exception Occurred: " << ex.what() << endl;
  }

  try {
	  con.executenonquery("create table IF NOT EXISTS detection_parameters(similarity_threshold FLOAT, false_negative_rate FLOAT)");
  }
  catch(exception &ex) {
	cerr << "Exception Occurred: " << ex.what() << endl;
  }


  
  try {
	  con.executenonquery("create table IF NOT EXISTS vector_generator_timing(file TEXT, total_wallclock FLOAT, total_usertime FLOAT, total_systime FLOAT, vecgen_wallclock FLOAT, vecgen_usertime FLOAT, vecgen_systime FLOAT )");
  }
  catch(exception &ex) {
	cerr << "Exception Occurred: " << ex.what() << endl;
  }

  try {
	  con.executenonquery("create table IF NOT EXISTS timing(property_name TEXT, total_wallclock FLOAT, total_usertime FLOAT, total_systime FLOAT, wallclock FLOAT, usertime FLOAT, systime FLOAT )");
  }
  catch(exception &ex) {
	cerr << "Exception Occurred: " << ex.what() << endl;
  }

  try {
    // Function IDs are only unique within a file
	  con.executenonquery("create table IF NOT EXISTS function_statistics(row_number INTEGER PRIMARY KEY, function_id INTEGER, num_instructions INTEGER )");
  }
  catch(exception &ex) {
	cerr << "Exception Occurred: " << ex.what() << endl;
  }

  try {
	  con.executenonquery("create table IF NOT EXISTS results(row_number INTEGER PRIMARY KEY, edit_distance INTEGER, false_positive_rate FLOAT )");
  }
  catch(exception &ex) {
	cerr << "Exception Occurred: " << ex.what() << endl;
  }

  try {
	  con.executenonquery("create table IF NOT EXISTS function_ids(row_number INTEGER PRIMARY KEY, file TEXT, function_name TEXT)");
  }
  catch(exception &ex) {
	cerr << "Exception Occurred: " << ex.what() << endl;
  }

  
  try {
    // index_within_function values are only unique within a file and function
//	  con.executenonquery("create table IF NOT EXISTS vectors(row_number INTEGER PRIMARY KEY, line INTEGER, offset INTEGER, sum_of_counts INTEGER, counts BLOB, instr_seq TEXT, function_id INTEGER, index_within_function INTEGER )");
	  con.executenonquery("create table IF NOT EXISTS vectors(row_number INTEGER PRIMARY KEY, function_id INTEGER,  index_within_function INTEGER, line INTEGER, offset INTEGER, sum_of_counts INTEGER, counts BLOB, instr_seq BLOB)");

  }
  catch(exception &ex) {
	cerr << "Exception Occurred: " << ex.what() << endl;
  }

  try {
    // index_within_function values are only unique within a file and function
	  con.executenonquery("create index IF NOT EXISTS vectors_sum_of_counts_index on vectors(sum_of_counts)");
  }
  catch(exception &ex) {
	cerr << "Exception Occurred: " << ex.what() << endl;
  }

  try {
	  con.executenonquery("create table IF NOT EXISTS clusters(row_number INTEGER PRIMARY KEY, cluster INTEGER, function_id INTEGER, index_within_function INTEGER, vectors_row INTEGER, dist INTEGER)");


  }
  catch(exception &ex) {
	cerr << "Exception Occurred: " << ex.what() << endl;
  }

  try {
	  con.executenonquery("create table IF NOT EXISTS postprocessed_clusters(row_number INTEGER PRIMARY KEY, cluster INTEGER, function_id INTEGER, index_within_function INTEGER, vectors_row INTEGER, dist INTEGER )");
  }
  catch(exception &ex) {
	cerr << "Exception Occurred: " << ex.what() << endl;
  }

  try {con.executenonquery("create table if not exists function_coverage(function_id integer, num_instructions_covered_not_postprocessed integer, fraction_instructions_covered_not_postprocessed float, num_instructions_covered_postprocessed integer, fraction_instructions_covered_postprocessed float)");}
  catch(exception &ex) {cerr << "Exception Occurred: " << ex.what() << endl;}

  try {con.executenonquery("create table if not exists total_coverage(num_instructions_covered_not_postprocessed integer, fraction_instructions_covered_not_postprocessed float, num_instructions_covered_postprocessed integer, fraction_instructions_covered_postprocessed float)");}
  catch(exception &ex) {cerr << "Exception Occurred: " << ex.what() << endl;}

  try {con.executenonquery("create index if not exists clusters_by_function on clusters(function_id)");}
  catch(exception &ex) {cerr << "Exception Occurred: " << ex.what() << endl;}

  try {con.executenonquery("create index if not exists postprocessed_clusters_by_function on postprocessed_clusters(function_id)");}
  catch(exception &ex) {cerr << "Exception Occurred: " << ex.what() << endl;}

  try {con.executenonquery("create index if not exists function_statistics_by_function on function_statistics(function_id)");}
  catch(exception &ex) {cerr << "Exception Occurred: " << ex.what() << endl;}

  try {
	sqlite3_command(con, "create index if not exists clusters_index_by_cluster on clusters(cluster)").executenonquery();
  } catch (exception& ex) {cerr << "Exception Occurred: " << ex.what() << endl;}
  try {
	sqlite3_command(con, "create index if not exists postprocessed_clusters_index_by_cluster on postprocessed_clusters(cluster)").executenonquery();
  } catch (exception& ex) {cerr << "Exception Occurred: " << ex.what() << endl;}

}

