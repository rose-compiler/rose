#include <mpi.h>
#include <rose.h>
#include "parallel_compass.h"


using namespace std;
#define DEBUG_OUTPUT_PC true
#define DEBUG_OUTPUT_PC_MORE false

using namespace Compass;



// ************************************************************
// NodeCounter to determine on how to split the nodes
// ************************************************************
class NodeNullDerefCounter: public AstSimpleProcessing
{
public:
  size_t totalNodes;
  size_t totalFunctions;
  size_t *nullDerefNodes;
  SgNode* *nodes;
  bool count;
  size_t interestingNodes;
  size_t nrOfForWhile;
  NodeNullDerefCounter(bool c) { interestingNodes=0; totalNodes=0; count=true; nrOfForWhile=1;}
  NodeNullDerefCounter(int numberOfNodes)
    : totalNodes(0), totalFunctions(0), 
      nullDerefNodes(new size_t[numberOfNodes+1]), fileptr(nullDerefNodes) {
    *fileptr = 0;
    interestingNodes=0; 
    count=false;
    nrOfForWhile=1;
    nodes = new SgNode*[numberOfNodes+1];
  }
protected:
  // increase the number of nodes and nodes in files
  virtual void visit(SgNode *node) {
    totalNodes++;
    if (!count) {
      ROSE_ASSERT(fileptr);
      ++*fileptr;
    }
    // must be run postorder!
    // which nodes are interesting? we need to perform the weighting on this.
    //    if (isSgArrowExp(node) || isSgPointerDerefExp(node)
    //	|| isSgAssignInitializer(node) || isSgFunctionCallExp(node)) {
    if (!count) {
      fileptr++;
      *fileptr = fileptr[-1];
      nodes[interestingNodes]=node;
      //if (DEBUG_OUTPUT_PC_MORE ) 
      //cerr << " [ " << interestingNodes << " ] adding node " << node->class_name() << endl;
    }
    interestingNodes++;
    //}
    if (isSgForStatement(node) || isSgWhileStmt(node) ||
	isSgVarRefExp(node) || isSgInitializedName(node) ||
	isSgFunctionCallExp(node))
      nrOfForWhile++;
    SgFunctionDeclaration *funcDecl = isSgFunctionDeclaration(node);
    if (funcDecl && funcDecl->get_definingDeclaration() == funcDecl) {
      totalFunctions++;
    }
  };
  size_t *fileptr;
};



// ************************************************************
// algorithm that splits the files to processors
// ************************************************************
std::pair<int, int> computeNullDerefIndices(SgProject *project, int my_rank, int processes, NodeNullDerefCounter nullderefCounter)
{
  nullderefCounter.traverse(project, postorder);
  int nrOfInterestingNodes = nullderefCounter.interestingNodes;
  // count the amount of nodes in all files
  //cerr << " number of interesting : " << nrOfInterestingNodes << endl;
  //  cerr << " nullderefCounter.nullDerefNodes[nrOfInterestingNodes ] == nullderefCounter.totalNodes  : " <<
  //  nullderefCounter.nullDerefNodes[nrOfInterestingNodes ] << " == " << nullderefCounter.totalNodes  << endl;
  ROSE_ASSERT(nullderefCounter.nullDerefNodes[nrOfInterestingNodes ] == nullderefCounter.totalNodes );

  if (my_rank == 0) {
    std::cerr <<  "0: File: The total amount of interesting nodes is : " << nrOfInterestingNodes << std::endl;
    std::cerr <<  "0: File: The total amount of functions is : " << nullderefCounter.totalFunctions << std::endl;
    std::cerr <<  "0: File: The total amount of nodes is : " << nullderefCounter.totalNodes << std::endl;
  }

  // split algorithm
  int lo=0, hi = 0;
  int my_lo=0, my_hi=0;
  for (int rank = 0; rank < processes; rank++) {
    const size_t my_nodes_high = (nullderefCounter.totalNodes / processes + 1) * (rank + 1);
    // set lower limit
    lo = hi;
    // find upper limit
    for (hi = lo + 1; hi < nrOfInterestingNodes; hi++) {
      if (nullderefCounter.nullDerefNodes[hi] > my_nodes_high)
	break;
    }

    // make sure all files have been assigned to some process
    if (rank == processes - 1)
      ROSE_ASSERT(hi == nrOfInterestingNodes);

    if (rank == my_rank)  {
      my_lo = lo;
      my_hi = hi;
#if 1
      std::cout << "process " << rank << ": files [" << lo << "," << hi
		<< "[ for a total of "
		<< (lo != 0 ? nullderefCounter.nullDerefNodes[hi-1] - nullderefCounter.nullDerefNodes[lo-1] : nullderefCounter.nullDerefNodes[hi-1])
		<< " nodes" << std::endl;
#endif
      break;
    }
  }


  return std::make_pair(my_lo, my_hi);
}



// ************************************************************
// algorithm that splits the files to processors
// ************************************************************
void computeIndicesPerNode(SgProject *project, std::vector<int>& nodeToProcessor,
                           int my_rank, int processes, 
			   NodeNullDerefCounter nullderefCounter,
			   std::vector<SgNode*>& nodeDecls)
{
  // nullderefCounter.traverse(project, postorder);
  int nrOfInterestingNodes = nullderefCounter.interestingNodes;
  // count the amount of nodes in all files
  //cerr << " number of interesting : " << nrOfInterestingNodes << endl;
  //  cerr << " nullderefCounter.nullDerefNodes[nrOfInterestingNodes ] == nullderefCounter.totalNodes  : " <<
  //  nullderefCounter.nullDerefNodes[nrOfInterestingNodes ] << " == " << nullderefCounter.totalNodes  << endl;
  ROSE_ASSERT(nullderefCounter.nullDerefNodes[nrOfInterestingNodes ] == nullderefCounter.totalNodes );

  if (my_rank == 0) {
    std::cout <<  "0: File: The total amount of interesting nodes is : " << nrOfInterestingNodes << std::endl;
    std::cout <<  "0: File: The total amount of functions is : " << nullderefCounter.totalFunctions << std::endl;
    std::cout <<  "0: File: The total amount of nodes is : " << nullderefCounter.totalNodes << std::endl;
  }

  //  std::vector<SgNode*> nodeDecls(nullderefCounter.totalNodes);
  std::vector<double> nodeWeights(nullderefCounter.totalNodes);

  std::vector<std::pair<double, size_t> > weights(nullderefCounter.totalNodes);
  for (unsigned int i=0; i< nullderefCounter.totalNodes; i++) {
    SgNode* node = nullderefCounter.nodes[i];
    double currentWeight = 0.0001;
    if (isSgArrowExp(node) || isSgPointerDerefExp(node)
    	|| isSgAssignInitializer(node) || isSgFunctionCallExp(node)) {
      currentWeight = 0.05;
    } else if (isSgFunctionDefinition(node) || isSgTemplateInstantiationMemberFunctionDecl(node)) {
      /*
	NodeNullDerefCounter nrNiF = NodeNullDerefCounter(true);
	nrNiF.traverse(node, postorder);
	currentWeight = 0.002*nrNiF.totalNodes*nrNiF.nrOfForWhile;
	// make sure a function is weighted more than all other nodes
	if (currentWeight<=0.05)
      */
      currentWeight=0.06;
    } else if (isSgFile(node)) {
      currentWeight=0.07;
    }
    weights[i].first = currentWeight;
    weights[i].second = i;
  }
  std::sort(weights.begin(), weights.end(), SortDescending());
  for (int i=0; i< (int) nullderefCounter.totalNodes; i++) {
    nodeDecls[i]=nullderefCounter.nodes[weights[i].second];
    nodeWeights[i]=weights[i].first;
    if (my_rank==0) {
      string name = "";
      string name2= "";
      SgFunctionDefinition* fast = isSgFunctionDefinition(nodeDecls[i]);
      if (fast) {
	name = fast->get_qualified_name().str();
	name2 = fast->get_declaration()->get_name().str();
      }
      if (DEBUG_OUTPUT_PC_MORE) 
	if (i<100)
	  std::cout << "    node : " << nodeDecls[i]->class_name() << 
	    "  weight : " << nodeWeights[i] << "   " << name << "  " << name2 <<  std::endl;

    }
  }


  int start_rank=1;
  if (processes==1)
    start_rank=0;

  double processorWeight[processes];
  int nrOfNodes[processes];
  for (int rank = 0; rank < processes; rank++) {
    processorWeight[rank] = 0;
    nrOfNodes[rank]=0;
  }

  for (unsigned int i=0; i< nullderefCounter.totalNodes; i++) {
    double currentWeight = nodeWeights[i];
    double min =INFINITY;
    int min_rank=start_rank;
    // find the minimum weight processor
    for (int rank = start_rank; rank < processes; rank++) {
      if (processorWeight[rank]<min) {
	min = processorWeight[rank];
	min_rank = rank;
      }
    }
    processorWeight[min_rank]+=currentWeight;
    nodeToProcessor.push_back(min_rank);
    nrOfNodes[min_rank]+=1;
    //      if (my_rank ==0 ) {
    //std::cout << " Nodes per Processor : " << node->class_name() << "  weight : " <<
    //  currentWeight /*<< "/" << totalWeight */<< "  on proc: " << min_rank << std::endl;
    //}
  }

  if (my_rank ==0 ) {
    cout << " " << endl;
    for (int rank = 0; rank < processes; rank++) {
      std::cout << "Processor : " << rank << "  has " << nrOfNodes[rank] <<
	" nodes. Processor weight: " << processorWeight[rank] << std::endl;
    }
  }

}



void printPCResults(CountingOutputObject  &outputs,
		    unsigned int* output_values,
		    double* times, double* memory, int* maxtime_i, double* maxtime_val,
		    double* calctimes, double* commtimes,
		    std::vector<SgNode*>& nodeDecls, double* totalCheckerTime
		    ) {
  /* print everything */
  if (my_rank == 0) {

    std::cout << "\n " << my_rank << " >>>>> results:" << std::endl;
    std::map<std::string, unsigned int> ::iterator o_itr;
    int j=0;
    for (o_itr = outputs.counts.begin(); o_itr != outputs.counts.end(); ++o_itr, ++j) 
      std::cout << "  " << o_itr->first << " " << output_values[j] << " " << totalCheckerTime[j] <<std::endl;
    std::cout << std::endl;

    double total_time = 0.0;
    double total_memory = 0.0;
    double min_time = std::numeric_limits<double>::max(), max_time = 0.0;
    //    int slowest_func=0;
    //int fastest_func=0;
    double total_calctimes=0;
    for (size_t i = 0; i < (size_t) processes; i++) {

      double maxval = maxtime_val[i];
      SgNode* n = isSgNode(nodeDecls[maxtime_i[i]]);
      string slow_node = n->class_name();
      SgFunctionDefinition* sl_node = isSgFunctionDefinition(n);
      if (sl_node) slow_node= sl_node->get_declaration()->get_name().str();

      int perc = 0;
      if (times[i]>0) 
	if (i==0 && processes>1)
	  perc = (int) (commtimes[i]/times[i]*10000);
	else {
	  perc = (int) (calctimes[i]/times[i]*10000);
	  total_calctimes += calctimes[i];
	}

      double perc_d = (double) perc/100;

      std::cout << "processor: " << i << " time: " << times[i] << "  memory: " << memory[i] <<  " MB " << 
	"  real # nodes:  " << (dynamicFunctionsPerProcessor[i]) << "   slowest node : " << slow_node << 
	"  maxtime: " << maxval << "  commtime : " << commtimes[i] << "  calctime : " << calctimes[i] << 
	"   => " << perc_d << " % "  << std::endl;


      total_time += times[i];
      total_memory += memory[i];
      if (min_time > times[i]) {
	min_time = times[i];
      }
      if (max_time < times[i]) {
	max_time = times[i];
      }

    }
    std::cout << std::endl;
    double d_perc=0;
    if (processes>1)
      d_perc =  total_calctimes/(total_time-times[0])*100;

    std::cerr << "\ntotal time: " << total_time << "   total memory : " << total_memory << " MB "
      	      << "\n    fastest process: " << min_time // << " fastest   in file: " << root->get_file(fastest_func).getFileName() 
      	      << "\n    slowest process: " << max_time //<< " slowest   in file: " << root->get_file(slowest_func).getFileName()
              << "\n    total calc time: " << d_perc << " % "
      //	      << "\n    fastest process: " << min_time  << " fastest " << fastest_func << "   in node: " << nodeDecls[fastest_func]->class_name() << ": " <<namef<< " : " <<namef2
      //              << "\n    slowest process: " << max_time  << " slowest node : " <<  nodeDecls[]->class_name() << ": " <<names<< " : " <<names2
	      << std::endl;
    std::cout << std::endl;
  
  }

}


// ************************************************************
// main function
// ************************************************************
int main(int argc, char **argv)
{
  gettime(begin_init_time);
  double mem_b = ROSE_MemoryUsage().getMemoryUsageMegabytes();
  /* setup MPI */
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &processes);
  
  std::cout << "Processor : " << my_rank << "  ROSE frontend .... " << std::endl;
  initPCompass(argc, argv, processes);
  ROSE_ASSERT(root);

  /* setup checkers */
  std::vector<AstSimpleProcessingWithRunFunction *> traversals;
  std::vector<GraphProcessingWithRunFunction *> traversalsGraph;
  std::vector<AstSimpleProcessingWithRunFunction *>::iterator t_itr;
  std::vector<GraphProcessingWithRunFunction *>::iterator t_itr_graph;
  std::vector<const Compass::Checker *> bases;
  std::vector<const Compass::Checker *> basesGraph;
  std::vector<const Compass::Checker *> basesAll;
  std::vector<const Compass::Checker *>::iterator b_itr;
  CountingOutputObject  outputs ;

  bool isBinaryInput=true;
  SgBinaryComposite* binFile = isSgBinaryComposite(root->get_fileList()[0]);

  //ROSE_ASSERT(binFile);
  SgAsmGenericFile* file = binFile != NULL ? binFile->get_binaryFile() : NULL;

  if (file==NULL)
    isBinaryInput=false;

  //  compassCheckers(traversals, bases, outputs);
  Compass::Parameters params(Compass::findParameterFile());
  buildCheckers(basesAll, params, outputs, root);

  for (b_itr = basesAll.begin(); b_itr != basesAll.end(); ++b_itr) {
      const Compass::CheckerUsingGraphProcessing* graphChecker = 
	dynamic_cast<const Compass::CheckerUsingGraphProcessing*>(*b_itr);
      if (graphChecker!=NULL) {
	//	bases.push_back(graphChecker);

	//	bool isBinary = graphChecker->isBinary();
	bool isbinary=false;
	int language = graphChecker->supportedLanguages;
	language >>= 4;
	int lan = language &0x1u ; // x86
	if (lan==1) isbinary=true;
	language >>= 1;
	lan = language &0x1u ;  // ARM
	if (lan==1) isbinary=true;
	if (my_rank==0)
	std::cerr << " found graph checker " << graphChecker->checkerName <<  " isBinaryChecker: " << isbinary ;

	if ((isbinary && isBinaryInput) || (!isbinary && !isBinaryInput)) {
	  basesGraph.push_back(graphChecker);
	  traversalsGraph.push_back(graphChecker->createGraphTraversal(params, &outputs));
	  if (my_rank==0)
	  cerr << " adding checker. " << endl;
	} else 	if (my_rank==0)
	  cerr <<  endl;
	
      }

      const Compass::CheckerUsingAstSimpleProcessing* astChecker = 
	dynamic_cast<const Compass::CheckerUsingAstSimpleProcessing*>(*b_itr);
      if (astChecker!=NULL) {

	bool isbinary=false;
	int language = astChecker->supportedLanguages;
	language >>= 4;
	int lan = language &0x1u ; // x86
	if (lan==1) isbinary=true;
	language >>= 1;
	lan = language &0x1u ;  // ARM
	if (lan==1) isbinary=true;
	if (my_rank==0)
	std::cerr << " found AST checker " << astChecker->checkerName << "  isBinaryChecker : " << isbinary ;

	if ((isbinary && isBinaryInput) || (!isbinary && !isBinaryInput)) {
	  bases.push_back(astChecker);
	  traversals.push_back(astChecker->createSimpleTraversal(params, &outputs));
	  if (my_rank==0)
	  cerr << " adding checker. " << endl;
	} else 	if (my_rank==0)
	  cerr  << endl;
      }
  }

  outputs.fillOutputList(bases);

  double* totalCheckerTime = new double[bases.size()];

  //ROSE_ASSERT(traversals.size() == bases.size() && bases.size() == outputs.size());
  //  if (DEBUG_OUTPUT_PC_MORE) 
  if (my_rank == 0)
    {
      std::cout << std::endl << "got " << bases.size() << " checkers:";
      for (b_itr = bases.begin(); b_itr != bases.end(); ++b_itr)
	std::cout << ' ' << (*b_itr)->checkerName;
      std::cout << std::endl;
    }

  for (b_itr = bases.begin(); b_itr != bases.end(); ++b_itr)
    Compass::runPrereqs(*b_itr, root);

  for (b_itr = basesGraph.begin(); b_itr != basesGraph.end(); ++b_itr)
    Compass::runPrereqs(*b_itr, root);

  // the following needs to be changed. Currently it takes a project as input
  // all graph analyses are run before the AsT traverals. Comment this out if not
  // needed for traversal.
  for ( b_itr = basesGraph.begin();b_itr!=basesGraph.end();++b_itr) { 
       //  for (t_itr_graph = traversalsGraph.begin(), b_itr = basesGraph.begin(); 
       //t_itr_graph != traversalsGraph.end(); ++t_itr_graph, ++b_itr) {
    ROSE_ASSERT(*b_itr);

    cerr << " Running GraphChecker on project... " << (*b_itr)->checkerName << endl;
    //ROSE_ASSERT(*t_itr_graph);
    // need to fix this
    //    (*t_itr_graph)->run(root);
    (*b_itr)->run(params, &outputs);
  }

  /* traverse the files */

  if (sequential)
    if (my_rank==0) {
      //      std::cout << "\n>>> Running in sequence ... " << std::endl;
      std::cout << "\n>>> Initializing data ... " << std::endl;
    }

  NodeNullDerefCounter nc(true);
  nc.traverse(root, postorder);
  int totalnr = nc.totalNodes;
  int nrOfInterestingNodes = nc.interestingNodes;
  NodeNullDerefCounter nullderefCounter(nrOfInterestingNodes);

  nullderefCounter.traverse(root, postorder);
  std::vector<SgNode*> nodeDecls(nullderefCounter.totalNodes);

  dynamicFunctionsPerProcessor = new int[processes];
  for (int k=0;k<processes;k++)
    dynamicFunctionsPerProcessor[k]=0;

#ifdef _OPENMP
  int threadsnr = -1;
#pragma omp parallel
  {
    // to set the amount of threads use: export OMP_NUM_THREADS=8
    //    omp_set_num_threads(8);
    threadsnr= omp_get_num_threads();
  }
  if (my_rank==0)
    std::cerr << "\n--------------- OPENMP enabled with " << threadsnr << 
      " threads!! processes = " << processes << " ------------" << std::endl;
#endif 


  //  if (!isBinaryInput)
  // ROSE_ASSERT(Compass::sourceDefUsePrerequisite.done==true);
  double mem_e = ROSE_MemoryUsage().getMemoryUsageMegabytes();
  double memuse = mem_e-mem_b;
  gettime(end_init_time);
  double init_time = timeDifference(end_init_time, begin_init_time);
  if (my_rank==0)
    std::cerr << " TIME FOR INIT IS : " << init_time << "  MEMORY USED : " << memuse << std::endl;


  // --------------------------------------------------------
  MPI_Barrier(MPI_COMM_WORLD);
  // this loop is used to get more than 1 result for a run ... to be able to average it out!!
  for (int count=0; count<1 ; count++) {

    double memusage_b = ROSE_MemoryUsage().getMemoryUsageMegabytes();

    // which node is the most expensive?
    int max_time_nr=0;
    double max_time=0;
    double calc_time_processor=0;

    std::vector<int> bounds;
    int t=0;
    for (b_itr = bases.begin(); b_itr != bases.end(); ++b_itr) {
      totalCheckerTime[t]=0;
      t++;
    }

    computeIndicesPerNode(root, bounds, my_rank, processes, nullderefCounter, nodeDecls);
    //  gettime(begin_time_0);
    gettime(begin_time);

    if (processes==1) {
      /* figure out which files to process on this process */
      if (DEBUG_OUTPUT_PC_MORE) 
	cout << "bounds size = " << bounds.size() << endl;
      int i=-1;
      std::set<std::string> foundFunction;
      std::set<std::string> foundFiles;
      gettime(begin_time_defuse);
#if ROSE_GCC_OMP
#pragma omp parallel for private(i,b_itr)  shared(bounds,my_rank,bases,nullderefCounter, foundFunction, foundFiles)
#endif
      for (i = 0; i<(int)bounds.size();i++) {
	if (DEBUG_OUTPUT_PC_MORE) 
	  cout << "bounds [" << i << "] = " << bounds[i] << "   my_rank: " << my_rank << endl;
	int t=0;
	b_itr=bases.begin();
	if (bounds[i]== my_rank) 
	  for (t_itr = traversals.begin(); t_itr != traversals.end(); ++t_itr, ++b_itr) {
	    SgNode* mynode = isSgNode(nullderefCounter.nodes[i]);
	    if (t_itr==traversals.begin()) {
	      if (isSgFunctionDeclaration(mynode)) { 
		SgFunctionDeclaration* def = isSgFunctionDeclaration(mynode);
		if (def->get_definition())
		  if (!def->get_file_info()->isCompilerGenerated()) {
		    std::string name = def->get_qualified_name();
		    Sg_File_Info* fileI = def->get_startOfConstruct();
		    std::string fileName = fileI->get_filenameString();
		    if (fileName.find("/usr/include")==string::npos) {		      
		      std::cerr << "  fname === " << name << "  " << fileName << std::endl;
		      //std::cout << "  name === " << name << std::endl;
		      foundFunction.insert(name);
		    } else {
		      //std::cerr << "  >> non local function: name === " << name << "  " << fileName << std::endl;
		    }
		  }
	      }
	      if (isSgFile(mynode)){
		SgFile* file = isSgFile(mynode);
		Sg_File_Info*  fileI = file->get_file_info();
		std::string fileName = fileI->get_filenameString();
		if (fileName.find("/usr/include")==string::npos) {
		  //		  std::cout << "  filename === "  << fileName << std::endl;
		  foundFiles.insert(fileName);
		} else {
		  //std::cerr << "  >> non local function: name === " << name << "  " << fileName << std::endl;
		}
	      }
	    }
	    ROSE_ASSERT(mynode);
	    gettime(begin_time_checker);
	    (*t_itr)->visit(mynode);
	    gettime(end_time_checker);
	    
	    double time_checker = Compass::timeDifference(end_time_checker, begin_time_checker);
	    totalCheckerTime[t]+=time_checker;
	    t++;
	  }
      }
      gettime(end_time_defuse);

      set<string>::const_iterator itF = foundFunction.begin();
      for (;itF!=foundFunction.end();++itF) {
	string function = *itF;
	std::cout << "  functionname === " << function << std::endl;
      }
      itF = foundFiles.begin();
      for (;itF!=foundFiles.end();++itF) {
	string function = *itF;
	std::cout << "  filename === " << function << std::endl;
      }


      std::cerr << "  parallel_compass : functions traversed = " << 
	RoseBin_support::ToString(foundFunction.size()) << "   " <<
	"  files traversed : " << RoseBin_support::ToString(foundFiles.size()) << std::endl;

      calc_time_processor = timeDifference(end_time_defuse, begin_time_defuse);
    } else {
      // apply runtime algorithm to def_use
      cout << " Dynamic scheduling ..." << endl;
      // next we need to communicate to 0 that we are ready, if so, 0 sends us the next job
      int currentJob = -1;
      MPI_Status Stat;
      int *res = new int[2];
      res[0]=5;
      res[1]=5;
      int *res2 = new int[2];
      res2[0]=-1;
      res2[1]=-1;
      bool done = false;
      int jobsDone = 0;
      // **********************************************************
      int scale = 2;
      // **********************************************************


      MPI_Request request[2]; 
      MPI_Status status[2];
      int min = -1;
      int max = -1;
      if (my_rank != 0) {
	//std::cout << " process : " << my_rank << " sending. " << std::endl;
	MPI_Send(res, 2, MPI_INT, 0, 1, MPI_COMM_WORLD);
	MPI_Recv(res, 2, MPI_INT, 0, 1, MPI_COMM_WORLD, &Stat);
	min = res[0];
	max = res[1];
	if (res[0]==-1) 
	  done =true;
      }

      while (!done) {
	// we are ready, make sure to notify 0
	double total_node=0;
	if (my_rank != 0) {
	  MPI_Isend(res, 2, MPI_INT, 0, 1, MPI_COMM_WORLD, &request[0]);
	  MPI_Irecv(res2, 2, MPI_INT, 0, 1, MPI_COMM_WORLD, &request[1]);

	  if (DEBUG_OUTPUT_PC_MORE) 	
	    std::cout << " process : " << my_rank << " receiving nr: [" << min << ":" << max << "[  of " << 
	      totalnr << "      range : " << (max-min);// << std::endl;

	  // OPENMP START-------------------------------------------------------
	  int i=-1;
	  gettime(begin_time_defuse);
#if ROSE_GCC_OMP
#pragma omp parallel for private(i,b_itr,begin_time_node,end_time_node)  shared(min,max,bases,nodeDecls,max_time,max_time_nr) reduction(+:total_node)
#endif
	  for (i=min; i<max;i++) { 
	    gettime(begin_time_node);
	    int t=0;
	    b_itr=bases.begin();
	    for (t_itr = traversals.begin(); t_itr != traversals.end(); ++t_itr,++b_itr) {
	      SgNode* mynode = isSgNode(nodeDecls[i]);
	      ROSE_ASSERT(mynode);
	      gettime(begin_time_checker);
	      (*t_itr)->visit(mynode);
	      gettime(end_time_checker);


	      double time_checker = Compass::timeDifference(end_time_checker, begin_time_checker);
	      totalCheckerTime[t]+=time_checker;
	      t++;
	    }
	    gettime(end_time_node);
	    double my_time_node = Compass::timeDifference(end_time_node, begin_time_node);
#if ROSE_GCC_OMP
#pragma omp critical (parallelcompassmulti)
#endif
	    if (my_time_node>max_time) {
	      max_time=my_time_node;
	      max_time_nr = i;
	    }
	  }
	  gettime(end_time_defuse);
	  total_node = timeDifference(end_time_defuse, begin_time_defuse);


	  // OPENMP END -------------------------------------------------------

	  calc_time_processor+=total_node;
	  if (DEBUG_OUTPUT_PC_MORE) 
	    std::cout << "     >>> Process " << my_rank << " done. Time: " << total_node << "   max_time : " << max_time << "  " << max_time_nr << 
	      "   in node : " << nodeDecls[max_time_nr]->class_name() << "  total_node: " << total_node << std::endl;


	  MPI_Waitall(2,request,status);
	  min = res2[0];
	  max = res2[1];
	  if (res2[0]==-1) 
	    break;
  
	}
	if (my_rank == 0) {
	  //std::cout << " process : " << my_rank << " receiving. " << std::endl;
	  MPI_Recv(res, 2, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &Stat);
	  gettime(begin_time_node);
	  currentJob+=scale;
	
	  // tps 31Jul2008 : best algorithm empirically found for batch distribution of jobs
	  int val=2;
	  /*
	    if (processes<64) {
	    if ((currentJob % 10)==9) scale+=(int)val;
	    } else
	    if ((currentJob % processes)==(processes-1)) scale=log(currentJob)+1;
	  */


	  if (count==0) {
	    if ((currentJob % processes)==(processes-1)) scale=log(currentJob)+1;
	  } else if (count==1) {
	    if ((currentJob % processes)==(processes-1)) scale=log(currentJob+10);
	  } else if (count==2) {
	    if (currentJob<10000)
	      if ((currentJob % processes)==(processes-1)) scale=log(currentJob+10);
	  } else  
	    if ((currentJob % 10)==9) scale+=(int)val;

	  if (currentJob>=(int)bounds.size()) {
	    res[0] = -1;
	    jobsDone++;
	  }      else {
	    res[0] = currentJob;
	    res[1] = currentJob+scale;
	    if (res[1]>=(int)bounds.size())
	      res[1] = bounds.size();
	    //cerr << " sending res[1] : " << res[1] << endl;
	    dynamicFunctionsPerProcessor[Stat.MPI_SOURCE] += scale;
	  }
	  //      std::cout << " processes done : " << jobsDone << "/" << (processes-1) << std::endl;
	  //std::cout << " process : " << my_rank << " sending rank : " << res[0] << std::endl;
	  gettime(end_time_node);
	  double my_time_node = timeDifference(end_time_node, begin_time_node);
	  total_node += my_time_node;
	  calc_time_processor+=total_node;
	  MPI_Send(res, 2, MPI_INT, Stat.MPI_SOURCE, 1, MPI_COMM_WORLD);      
	  if (jobsDone==(processes-1))
	    break;
	}
      }
      if (my_rank==0)
	cerr << ">>> Final scale = " << scale << endl; 
      //"  count = " << count << "  val = " << val << endl;
    }


    gettime(end_time);
    double memusage_e = ROSE_MemoryUsage().getMemoryUsageMegabytes();
    double memusage = memusage_e-memusage_b;
    double my_time = timeDifference(end_time, begin_time);
    double commtime = my_time-calc_time_processor;
    std::cout << ">>> Process " << my_rank << " is done. Time: " << my_time << "  Memory: " << memusage << " MB." << 
      "    CalcTime: " << calc_time_processor << "   CommTime: " << commtime << std::endl;

    unsigned int *output_values = new unsigned int[outputs.counts.size()];
    double *times = new double[processes];
    double *memory = new double[processes];
    int *maxtime_nr = new int[processes];
    double *maxtime_val = new double[processes];
    double *calctimes = new double[processes];
    double *commtimes = new double[processes];
    MPI_Barrier(MPI_COMM_WORLD);

    communicateResult(outputs, 
		      times,  memory, output_values, my_time, memusage, 
		      maxtime_nr, max_time_nr, 
		      maxtime_val, max_time,
		      calctimes, calc_time_processor, 
		      commtimes, commtime);



    printPCResults(outputs, output_values, times, memory, maxtime_nr, maxtime_val, 
                   calctimes, commtimes, nodeDecls, totalCheckerTime);


    //if (my_rank==0)
    //  cout << "Processor 0 : total time (incl. gathering) : " << my_time_0 << endl << endl;
    delete[] output_values;
    delete[] times;
    delete[] memory;
    delete[] maxtime_nr;
    delete[] maxtime_val;
    delete[] calctimes;
    delete[] commtimes;
  }
    delete[] totalCheckerTime;
    totalCheckerTime=NULL;


  /* all done */
  MPI_Finalize();

}


