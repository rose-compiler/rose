#include "SettingUpLSH.h"
using namespace boost::program_options;
using namespace std;
using namespace sqlite3x;
using boost::scoped_array;


// This is from callLSH.C, with the corresponding version for postprocessed_clusters added
static 
void insert_into_clusters(
    sqlite3_connection& con,
    int cluster,  
    int function_id, 
    int index_within_function,
    int vectors_row, 
    int dist 
    ) {
  std::string db_select_n = "INSERT INTO clusters(cluster, function_id, index_within_function, vectors_row, dist) VALUES(?,?,?,?,?)";
  try{
    //sqlite3_transaction trans2(con);
    {
      sqlite3_command cmd(con, db_select_n.c_str());
      cmd.bind(1,cluster);
      cmd.bind(2,function_id);
      cmd.bind(3,index_within_function);
      cmd.bind(4,vectors_row);
      cmd.bind(5,dist);
      cmd.executenonquery();
    }
    //trans2.commit();
  }
  catch(exception &ex) {
    cerr << "Exception Occurred: " << ex.what() << endl;
  }
}

static 
void insert_into_postprocessed_clusters(
    sqlite3_connection& con,
    int cluster,  
    int function_id, 
    int index_within_function,
    int vectors_row, 
    int dist 
    ) {
  std::string db_select_n = "INSERT INTO postprocessed_clusters(cluster, function_id, index_within_function, vectors_row, dist) VALUES(?,?,?,?,?)";
  try{
    //sqlite3_transaction trans2(con);
    {
      sqlite3_command cmd(con, db_select_n.c_str());
      cmd.bind(1,cluster);
      cmd.bind(2,function_id);
      cmd.bind(3,index_within_function);
      cmd.bind(4,vectors_row);
      cmd.bind(5,dist);
      cmd.executenonquery();
    }
    //trans2.commit();
  }
  catch(exception &ex) {
    cerr << "Exception Occurred: " << ex.what() << endl;
  }
}



/*
  Prints the usage of the LSHMain.
 */
void usage(int code, char *programName) {
  printf("Usage: %s [options: see source code] data_set_file [params_file]\n", programName);
  exit(code);
}

void readDataSetFromDatabase(sqlite3_connection& con,  scoped_array<PPointT>& dataSetPoints,   IntT& nPoints,  IntT& pointsDimension, const std::string& groupLow, const std::string& groupHigh) 
{

  long long eltCount = 0;
  try {
    sqlite3_command cmd(con,
        atoi(groupHigh.c_str()) == -1
        ? "SELECT count(*) from vectors where sum_of_counts >= ?"
        : "SELECT count(*) from vectors where sum_of_counts >= ? and sum_of_counts <= ?");

    cmd.bind(1,groupLow);
    if( atoi(groupHigh.c_str()) != -1 )
      cmd.bind(2,groupHigh);

    eltCount = cmd.executeint64();
  } catch (exception& e) {cerr << "Exception: " << e.what() << endl;}

  if (eltCount == 0) {
    cerr << "No vectors found -- invalid database?" << endl;
    exit (1);
  }

  cerr << "Found " << eltCount << " vectors" << endl;
  
  dataSetPoints.reset(new PPointT[eltCount]);
  nPoints = eltCount;
  size_t numActualPoints = 0;
  
  try {
    sqlite3_command cmd(con,
        atoi(groupHigh.c_str()) == -1
        ? "SELECT row_number, function_id, index_within_function, line, offset, counts, instr_seq from vectors where sum_of_counts >= ?"
        : "SELECT row_number, function_id, index_within_function, line, offset, counts, instr_seq from vectors where sum_of_counts >= ? and sum_of_counts <= ?");

    cmd.bind(1,groupLow);
    if( atoi(groupHigh.c_str()) != -1 )
      cmd.bind(2,groupHigh);

    sqlite3_reader r = cmd.executereader();
    while (r.read()) {
      long long rowNumber = r.getint64(0);
      int functionId = r.getint(1);
      int indexWithinFunction = r.getint(2);
      long long line = r.getint64(3);
      int offset = r.getint(4);
      string compressedCounts = r.getblob(5);
      string instrSeqMD5 = r.getblob(6);
      if (numActualPoints >= eltCount) {
        cerr << "Row number in database is out of range" << endl;
        abort();
      }
      dataSetPoints[numActualPoints].reset(new PointT);
      PointT& ve = *dataSetPoints[numActualPoints];
      ve.index = numActualPoints;
      ve.coordinatesC = vector<uint8_t>(compressedCounts.begin(), compressedCounts.end());
      ve.sqrLength = l2normSquared(&ve.coordinatesC[0], ve.coordinatesC.size());
      ve.prop.rowNumber = rowNumber;
      ve.prop.functionId = functionId;
      ve.prop.indexWithinFunction = indexWithinFunction;
      ve.prop.line = line;
      ve.prop.offset = offset;
      if (instrSeqMD5.size() != 16) {
        cerr << "Found MD5 with length other than 16" << endl;
        abort();
      }
      memcpy(ve.prop.instrSeqMD5, instrSeqMD5.data(), 16);
      ++numActualPoints;
      if (numActualPoints % 100000 == 0) cerr << "Got row " << numActualPoints << endl;
    }
  } catch (exception& e) {cerr << "Exception: " << e.what() << endl;}

  if (dataSetPoints[0]->coordinatesC.size() == 0) {
    cerr << "Vector slot 0 is empty" << endl;
    abort();
  }
  nPoints = numActualPoints;
  pointsDimension = getUncompressedSizeOfVector(&dataSetPoints[0]->coordinatesC[0], dataSetPoints[0]->coordinatesC.size());

  cerr << "Vectors have " << pointsDimension << " elements" << endl;
};



#define CHECK_INT(v) { \
  if (v == 0) { \
    fprintf(stderr, "Incorrect value for %s\n", __STRING(v)); \
    usage(1, argv[0]); \
  }}



bool readParamsFile(char *paramsFile, RNNParametersT*& algParameters, PRNearNeighborStructT*& nnStructs, IntT& nRadii, IntT& nPoints, IntT& pointsDimension, scoped_array<PPointT>& dataSetPoints, RealT*& listOfRadii  )
{
  FILE *pFile = fopen(paramsFile, "rt");
  if (pFile == NULL) {
    fprintf(stderr,
	    "Warning: could not open %s, will try to compute parameters "
	    "and write them to that file\n", paramsFile);
    perror(paramsFile);
    return true;
  } else {
    fscanf(pFile, "%ld\n", &nRadii);
    fprintf(stderr, "Using the following R-NN DS parameters (from %s):\n", paramsFile);
    fprintf(stderr, "N radii = %ld, nPoints = %ld\n", nRadii, nPoints);
    nnStructs = new PRNearNeighborStructT[nRadii];
    algParameters = new RNNParametersT[nRadii];
    for(IntT i = 0; i < nRadii; i++){
      algParameters[i] = readRNNParameters(pFile);
      printRNNParameters(stderr, algParameters[i]);
      nnStructs[i] = initLSH_WithDataSet(algParameters[i], nPoints, &dataSetPoints[0]);
      ASSERT(nnStructs[i] != NULL);
    }
    
    pointsDimension = algParameters[0].dimension;
    //if (listOfRadii != NULL) FREE(listOfRadii);
    listOfRadii = new RealT[nRadii];

    for(IntT i = 0; i < nRadii; i++){
      listOfRadii[i] = algParameters[i].parameterR;
    }
    return false;
  }
}


void doLSH(int argc, char* argv[])
{
  RNNParametersT *algParameters = NULL;
  PRNearNeighborStructT *nnStructs = NULL;

  // The data set containing all the points.
  scoped_array<PPointT> dataSetPoints;
  //PPointT *dataSetPoints = NULL;
  // Number of points in the data set.
  IntT nPoints = 0;
  // The dimension of the points.
  IntT pointsDimension = 0;

  // The succes probability of each point (each near neighbor is
  // reported by the algorithm with probability <successProbability>).
  RealT successProbability = 0.9;

  // Same as <thresholdR>, only an array of R's (for the case when
  // multiple R's are specified).
  RealT *listOfRadii = NULL;
  IntT nRadii = 0;

  RealT *memRatiosForNNStructs = NULL;


  
  //initializeLSHGlobal();
  availableTotalMemory = 800000000;
   
  callIsDistSqrLen = 0;
  callDistance = 0;
  
  // Parse part of the command-line parameters.
  bool computeParameters = false;
  char *paramsFile = NULL;

  int upperBound = 0, lowerBound = 2;
  int minNumNodes = 0, min_nVars = 0;
  int max_num_diff_vars = 16;
  float max_num_diff_nVars_diff = 0.5, max_nVars_diff = 0.35;
  int min_lines = 0;

  sqlite3_connection con;
 



 
  std::string databaseName;
  std::string groupLow;
  std::string groupHigh;
  for (int opt; (opt = getopt(argc, argv, "g:G:l:v:V:e:E:m:p:P:R:M:cf:b:t:")) != -1; ) {
    // Needed: -p -f -R
    switch (opt) {
      case 'g' : groupLow = optarg; break;
      case 'G' : groupHigh = optarg; break;
      case 'l': min_lines = atoi(optarg); break;
      case 'v': min_nVars = atoi(optarg); break;
      case 'V': max_num_diff_vars = atoi(optarg); break;
      case 'e': max_num_diff_nVars_diff = atof(optarg); break;
      case 'E': max_nVars_diff = atof(optarg); break;
      case 'm': minNumNodes = atoi(optarg); break;
      case 'b': lowerBound = atoi(optarg); break;
      case 't': upperBound = atoi(optarg); break;
      case 'p': paramsFile = optarg; break;
      case 'P': successProbability = atof(optarg); break;
      case 'M': availableTotalMemory = atol(optarg); break;
      case 'c':
                fprintf(stderr, "Warning: will compute parameters\n");
                computeParameters = true;
                break;
      case 'R':
                nRadii = 1;
                listOfRadii = new RealT[nRadii];
                memRatiosForNNStructs = new RealT[nRadii];
                listOfRadii[0] = strtod(optarg, NULL);
                memRatiosForNNStructs[0] = 1;
                break;
      case 'f':
                databaseName=optarg;
                DPRINTF("Allocated memory (after reading data set): %ld\n", totalAllocatedMemory);
                break;
      default:
                fprintf(stderr, "Unknown option: -%c\n", opt);
                usage(1, argv[0]);
    }
  }
 
  con.open(databaseName.c_str());
  readDataSetFromDatabase(con,dataSetPoints, nPoints, pointsDimension, groupLow,groupHigh);

  // Setup stuff for postprocessing
  int windowSize = 0;
  int stride = 0;
  try {
    windowSize = sqlite3_command(con, "select window_size from run_parameters limit 1").executeint();
  } catch (exception& ex) {cerr << "Exception Occurred: " << ex.what() << endl;}
  try {
    stride = sqlite3_command(con, "select stride from run_parameters limit 1").executeint();
  } catch (exception& ex) {cerr << "Exception Occurred: " << ex.what() << endl;}
  assert (windowSize != 0);
  assert (stride != 0);

  const size_t numStridesThatMustBeDifferent = windowSize / (stride * 2);





  
  if (optind < argc) {
    fprintf(stderr, "There are unprocessed parameters left\n");
    usage(1, argv[0]);
  }

  CHECK_INT(availableTotalMemory);
  CHECK_INT(nRadii);

  if (computeParameters == false)
    computeParameters = readParamsFile(paramsFile, algParameters, nnStructs, nRadii, nPoints,  pointsDimension, dataSetPoints, listOfRadii);

  if (computeParameters) {
    IntT nSampleQueries = N_SAMPLE_QUERY_POINTS;
    PPointT sampleQueries[nSampleQueries];
    IntT sampleQBoundaryIndeces[nSampleQueries];
    
    // Choose several data set points for the sample query points.
    for(IntT i = 0; i < nSampleQueries; i++){
      int random =genRandomInt(0, nPoints - 1);
      assert(random < nPoints);
        
      sampleQueries[i] = dataSetPoints[random];
    }

    // Compute the array sampleQBoundaryIndeces that specifies how to
    // segregate the sample query points according to their distance
    // to NN.
    sortQueryPointsByRadii(pointsDimension,
			   nSampleQueries,
			   sampleQueries,
			   nPoints,
			   &dataSetPoints[0],
			   nRadii,
			   listOfRadii,
			   sampleQBoundaryIndeces);


    // Compute the R-NN DS parameters
    // if a parameter file is given, output them to that file, and continue
    // otherwise, output them to stdout, and exit

    FILE *fd;
    if (paramsFile == NULL) {
      fd = stdout;
    } else {
      fd = fopen(paramsFile, "wt");
      if (fd == NULL) {
	fprintf(stderr, "Unable to write to parameter file %s\n", paramsFile);
	exit(1);
      }
    }

    fprintf(fd, "%ld\n", nRadii);

    // Tranforming <memRatiosForNNStructs> from
    // <memRatiosForNNStructs[i]=ratio of mem/total mem> to
    // <memRatiosForNNStructs[i]=ratio of mem/mem left for structs i,i+1,...>.

    RealT sum = 0;
    for(IntT i = nRadii - 1; i >= 0; i--){
      sum += memRatiosForNNStructs[i];
      memRatiosForNNStructs[i] = memRatiosForNNStructs[i] / sum;
      //DPRINTF("%0.6lf\n", memRatiosForNNStructs[i]);
    }
    ASSERT(sum <= 1.000001);

    //DONE Transforming

    
    for(IntT i = 0; i < nRadii; i++) {
      // which sample queries to use
      IntT segregatedQStart = (i == 0) ? 0 : sampleQBoundaryIndeces[i - 1];
      IntT segregatedQNumber = nSampleQueries - segregatedQStart;
      if (segregatedQNumber == 0) {
	// XXX: not the right answer
	segregatedQNumber = nSampleQueries;
	segregatedQStart = 0;
      }
      ASSERT(segregatedQStart < nSampleQueries);
      ASSERT(segregatedQStart >= 0);
      ASSERT(segregatedQStart + segregatedQNumber <= nSampleQueries);
      ASSERT(segregatedQNumber >= 0);
      RNNParametersT optParameters = computeOptimalParameters(listOfRadii[i],
							      successProbability,
							      nPoints,
							      pointsDimension,
							      &dataSetPoints[0],
							      segregatedQNumber,
							      sampleQueries + segregatedQStart,
							      (UnsT)((availableTotalMemory - totalAllocatedMemory) * memRatiosForNNStructs[i]));
      printRNNParameters(fd, optParameters);
    }
    if (fd == stdout) {
      exit(0);
    } else {
      fclose(fd);
      system((std::string("ls -l ") + paramsFile).c_str());
      if( readParamsFile(paramsFile, algParameters, nnStructs, nRadii, nPoints,  pointsDimension, dataSetPoints, listOfRadii) == true )
      {
        std::cerr << "The paramater file was not correctly generated." << std::endl;
        exit(1);
      }
      // ASSERT(readParamsFile(paramsFile) == false);
    }
  }

  // output vector clusters according to the filtering parameters.
  printf("========================= Structure built =========================\n");
  printf("nPoints = %ld, Dimension = %ld\n", nPoints, pointsDimension);
  printf("*** Filtering Parameters for individual vectors ***\n");
  printf("minNumNodes = %d, min_nVars = %d, min_lines = %d\n", minNumNodes, min_nVars, min_lines);
  printf("*** Filtering Parameters for clusters ***\n");
  printf("lowerBound = %d, upperBound = %d\n", lowerBound, upperBound);
  printf("Max num of different nVars = %d, Max diff among different nVars = %g, \nMax diff among the num of different nVars = %g\n", max_num_diff_vars, max_nVars_diff, max_num_diff_nVars_diff);


  vector<PPointT> result;
  PPointT queryPoint;

  TimeVarT meanQueryTime = 0;
  IntT nQueries = 0;
  bool seen[nPoints];
  IntT nBuckets = 0, nBucketedPoints = 0;

  memset(seen, 0, nPoints * sizeof(bool));
  for(IntT i = 0; i < nPoints; nQueries++, i++) {

    // find the next unseen point
    while (i < nPoints && seen[i]) i++;
    if (i >= nPoints) break;
    queryPoint = dataSetPoints[i];
    // get the near neighbors.
    IntT nNNs = 0;

    for(IntT r = 0; r < nRadii; r++) { // nRadii is always 1 so far.
      ASSERT(nnStructs[r] != NULL);
      nNNs = getRNearNeighbors(nnStructs[r], queryPoint, result);

      //printf("Total time for R-NN query at radius %0.6lf (radius no. %ld):\t%0.6lf\n", (double)(listOfRadii[r]), r, timeRNNQuery);
      meanQueryTime += timeRNNQuery;

      //printf("\nQuery point %ld: found %ld NNs at distance %0.6lf (radius no. %ld). NNs are:\n",
      //       i, nNNs, (double)(listOfRadii[r]), r);

      // The result array may contain the queryPoint, so do not output it in the following.

      if ( nNNs>=lowerBound ) { // filter out non-clones anyway
        nBuckets++;
        printf("\n");
        for (PPointT *p = &result[0]; p != &result[nNNs]; p++)  {
          ASSERT (*p != NULL);
          nBucketedPoints++;
          seen[(*p)->index] = true;

          const VectorEntry& ve = (*p)->prop;

          insert_into_clusters(con, nBuckets, ve.functionId, ve.indexWithinFunction, (*p)->prop.rowNumber, 1);

          
        } // end of enumeration of a bucket


        
      } // end of nNNs>=lowerBound
    } // for (...nRadii...)
  }

  // Simple statistics and finish
  if (nQueries > 0) {
    meanQueryTime = meanQueryTime / nQueries;
    printf("\n%ld queries, Mean query time: %0.6lf\n", nQueries, (double)meanQueryTime);
    printf("%ld buckets, %ld points (out of %ld, %.2f %%) in them\n",
	   nBuckets, nBucketedPoints, nPoints, 100*(float)nBucketedPoints/(float)nPoints);
  } else {
    printf("No query\n");
  }

  std::cout << "Called IsDistanceSqrLen: " << callIsDistSqrLen << std::endl;
  std::cout << "Called distance: " << callDistance << std::endl;



  //freePRNearNeighborStruct(nnStruct);

};


