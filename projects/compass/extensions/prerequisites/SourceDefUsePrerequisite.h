#ifndef COMPASS_SOURCE_DEFUSE_PREREQUISITE_H
#define COMPASS_SOURCE_DEFUSE_PREREQUISITE_H

// The prerequisite for getting the SgProject
class SourceDefUsePrerequisite: public Prerequisite 
{
  private:
    DefUseAnalysis* defuse;
  public:
    SourceDefUsePrerequisite(): Prerequisite("SourceDefUse"), defuse(NULL) {}

    void load(SgProject* root) 
    {
      done=true;
      defuse = new DefUseAnalysis(root);

    } //load(SgProject* root)

    void run(SgProject* root) {
      //      if (done) return;
      done =true;
  if (defuse==NULL) {
    //#define DEFUSE
    std::cerr << " DEFUSE ==NULL ... running defuse analysis " << std::endl;
#if ROSE_MPI
    std::cerr << " MPI is enabled! Running in parallel ... " << std::endl;
    /* ----------------------------------------------------------
     * MPI code for DEFUSE
     * ----------------------------------------------------------*/
    // --------------------------------------------------------
    // (tps, 07/24/08): added support for dataflow analysis
    // this should run right before any other checker is executed.
    // Other checkers rely on this information.
    struct timespec begin_time_node, end_time_node;
    struct timespec b_time_node, e_time_node;

    // create map with all nodes and indices
    MemoryTraversal* memTrav = new MemoryTraversal();
    memTrav->traverseMemoryPool();
    std::cerr << my_rank << " >> MemoryTraversal - Elements : " << memTrav->counter << std::endl;
    ROSE_ASSERT(memTrav->counter>0);
    ROSE_ASSERT(memTrav->counter==memTrav->nodeMap.size());

    MPI_Barrier(MPI_COMM_WORLD);
    gettime(begin_time_node);

    if (defuse==NULL)
      defuse = new DefUseAnalysis(root);

    //defuse->disableVisualization();
    Rose_STL_Container<SgNode *> funcs =
      NodeQuery::querySubTree(root, V_SgFunctionDefinition);
    if (my_rank==0)
      std::cerr << "\n>>>>> running defuse analysis (with MPI)...  functions: " << funcs.size() <<
        "  processes : " << processes << std::endl;
    int resultDefUseNodes=0;


    // ---------------- LOAD BALANCING of DEFUSE -------------------
    // todo: try to make the load balancing for DEFUSE better
    FunctionNamesPreTraversal preTraversal;
    MyAnalysis myanalysis;
    int initialDepth=0;
    std::vector<int> bounds;

    myanalysis.computeFunctionIndicesPerNode(root, bounds, initialDepth, &preTraversal);

    if (processes==1) {
      for (int i = 0; i<(int)bounds.size();i++) {
        //std::cerr << "bounds [" << i << "] = " << bounds[i] << "   my_rank: " << my_rank << std::endl;
        if (bounds[i]== my_rank) {

          SgFunctionDeclaration* funcDecl = myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls[i];
          SgFunctionDefinition* funcDef = NULL;
          if (funcDecl)
            funcDef = funcDecl->get_definition();
          if (funcDef) {
            int nrNodes = ((DefUseAnalysis*)defuse)->start_traversal_of_one_function(funcDef);
            resultDefUseNodes+=nrNodes;
          }
        }
      }
    } else {
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
      int scale = 1;
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

          if (((max-min) % 20)==0 || min <10)
            std::cout << " process : " << my_rank << " receiving nr: [" << min << ":" << max << "[ of " <<
              bounds.size() << "     range : " << (max-min) << std::endl;

          for (int i=min; i<max;i++) {
            SgNode* mynode = isSgNode(myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls[i]);
            ROSE_ASSERT(mynode);
            SgFunctionDeclaration* funcDecl = isSgFunctionDeclaration(mynode);
            SgFunctionDefinition* funcDef = NULL;
            if (funcDecl)
              funcDef = funcDecl->get_definition();
            if (funcDef) {
              int nrNodes = ((DefUseAnalysis*)defuse)->start_traversal_of_one_function(funcDef);
              resultDefUseNodes+=nrNodes;
            }
          }

          MPI_Waitall(2,request,status);
          min = res2[0];
          max = res2[1];
          if (res2[0]==-1)
            break;
        } // if
        if (my_rank == 0) {
          //std::cout << " process : " << my_rank << " receiving. " << std::endl;
          MPI_Recv(res, 2, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &Stat);
          gettime(begin_time_node);
          currentJob+=scale;

          if ((currentJob % 10)==9) scale+=1;
          if (currentJob>=(int)bounds.size()) {
            res[0] = -1;
            jobsDone++;
          }      else {
            res[0] = currentJob;
            res[1] = currentJob+scale;
            if (res[1]>=(int)bounds.size())
              res[1] = bounds.size();
            //      dynamicFunctionsPerProcessor[Stat.MPI_SOURCE] += scale;
          }
          //      std::cout << " processes done : " << jobsDone << "/" << (processes-1) << std::endl;
          //std::cout << " process : " << my_rank << " sending rank : " << res[0] << std::endl;
          gettime(end_time_node);
          double my_time_node = timeDifference(end_time_node, begin_time_node);
          total_node += my_time_node;
          //calc_time_processor+=total_node;
          MPI_Send(res, 2, MPI_INT, Stat.MPI_SOURCE, 1, MPI_COMM_WORLD);
          if (jobsDone==(processes-1))
            break;
        }

      } //while
      if (my_rank==0)
        std::cerr << ">>> Final scale = " << scale << std::endl;

    } //else
    // ---------------- LOAD BALANCING of DEFUSE -------------------


    gettime(end_time_node);
    double my_time_node = timeDifference(end_time_node, begin_time_node);
    std::cerr << my_rank << ": DefUse Analysis complete. Nr of nodes: " << resultDefUseNodes <<
      "  time: " << my_time_node << std::endl;
    MPI_Barrier(MPI_COMM_WORLD);
    if (my_rank==0)
      std::cerr << "\n>> Collecting defuse results ... " << std::endl;


    ROSE_ASSERT(defuse);
    my_map defmap = defuse->getDefMap();
    my_map usemap = defuse->getUseMap();
    std::cerr << my_rank << ": Def entries: " << defmap.size() << "  Use entries : " << usemap.size() << std::endl;
    gettime(end_time_node);
    my_time_node = timeDifference(end_time_node, begin_time_node);


    /* communicate times */
    double *times_defuse = new double[processes];
    MPI_Gather(&my_time_node, 1, MPI_DOUBLE, times_defuse, 1, MPI_DOUBLE, 0,
               MPI_COMM_WORLD);
    double totaltime=0;
    for (int i=0;i<processes;++i)
      if (times_defuse[i]>totaltime)
        totaltime=times_defuse[i];
    delete[] times_defuse;

    if (my_rank==0) {
      std::cerr << ">> ---- Time (max) needed for DefUse : " << totaltime << std::endl <<std::endl;
      Compass::gettime(begin_time_node);
      Compass::gettime(b_time_node);
    }
    //((DefUseAnalysis*)defuse)->printDefMap();
    /* communicate times */

    /* communicate arraysizes */
    unsigned int arrsize = 0;
    unsigned int arrsizeUse = 0;
    my_map::const_iterator dit = defmap.begin();
    for (;dit!=defmap.end();++dit) {
      arrsize +=(dit->second).size()*3;
    }
    my_map::const_iterator dit2 = usemap.begin();
    for (;dit2!=usemap.end();++dit2) {
      arrsizeUse +=(dit2->second).size()*3;
    }
    std::cerr << my_rank << ": defmapsize : " << defmap.size() << "  usemapsize: " << usemap.size()
              << ": defs : " << arrsize << "  uses: " << arrsizeUse << std::endl;
    // communicate total size to allocate global arrsize
    global_arrsize = -1;
    global_arrsizeUse = -1;
    MPI_Allreduce(&arrsize, &global_arrsize, 1, MPI_UNSIGNED, MPI_SUM, MPI_COMM_WORLD);
    MPI_Allreduce(&arrsizeUse, &global_arrsizeUse, 1, MPI_UNSIGNED, MPI_SUM, MPI_COMM_WORLD);
    /* communicate arraysizes */

    /* serialize all results */
    def_values_global = new unsigned int[global_arrsize];
    unsigned int *def_values =new unsigned int[arrsize];
    use_values_global = new unsigned int[global_arrsizeUse];
    unsigned int *use_values =new unsigned int[arrsizeUse];
    for (unsigned int i=0; i<arrsize;++i)
      def_values[i]=0;
    for (unsigned int i=0; i<global_arrsize;++i)
      def_values_global[i]=0;
    for (unsigned int i=0; i<arrsizeUse;++i)
      use_values[i]=0;
    for (unsigned int i=0; i<global_arrsizeUse;++i)
      use_values_global[i]=0;

    serializeDefUseResults(def_values, defmap, memTrav->nodeMapInv);
    serializeDefUseResults(use_values, usemap, memTrav->nodeMapInv);
    /* serialize all results */
    std::cerr << my_rank << " : serialization done."  << std::endl;

    MPI_Barrier(MPI_COMM_WORLD);
    if (my_rank==0) {
      Compass::gettime(e_time_node);
      double restime = Compass::timeDifference(e_time_node, b_time_node);
      std::cerr << "\n >>> serialization done. TIME : " << restime << std::endl;
      Compass::gettime(b_time_node);
    }

    /* communicate all results */
    int* offset=new int[processes];
    int* length=new int[processes];
    int* global_length=new int [processes];
    int* offsetUse=new int[processes];
    int* lengthUse=new int[processes];
    int* global_lengthUse=new int [processes];
    MPI_Allgather(&arrsize, 1, MPI_INT, global_length, 1, MPI_INT, MPI_COMM_WORLD);
    MPI_Allgather(&arrsizeUse, 1, MPI_INT, global_lengthUse, 1, MPI_INT, MPI_COMM_WORLD);

    for (int j=0;j<processes;++j) {
      if (j==0) {
        offset[j]=0;
        offsetUse[j]=0;
      } else {
        offset[j]=offset[j - 1] + global_length[j-1];
        offsetUse[j]=offsetUse[j - 1] + global_lengthUse[j-1];
      }
      length[j]=global_length[j];
      lengthUse[j]=global_lengthUse[j];
    }
    std::cerr << my_rank << " : serialization done."
              <<  "  waiting to gather...   arrsize: " << arrsize << "  offset : " << offset[my_rank] << " globalarrsize: " << global_arrsize<< std::endl;

    delete[] global_length;
    delete[] global_lengthUse;

    MPI_Barrier(MPI_COMM_WORLD);
    if (my_rank==0) {
      Compass::gettime(e_time_node);
      double restime = Compass::timeDifference(e_time_node, b_time_node);
      std::cerr << "\n >>> communication (ARRSIZE) done. TIME : " << restime << "  BROADCASTING ... " << std::endl;
      Compass::gettime(b_time_node);
    }


    // tps: This version seems slightly faster than the one following
    MPI_Allgatherv(def_values, arrsize, MPI_UNSIGNED, def_values_global, length,
                   offset, MPI_UNSIGNED,  MPI_COMM_WORLD);
    MPI_Allgatherv(use_values, arrsizeUse, MPI_UNSIGNED, use_values_global, lengthUse,
                   offsetUse, MPI_UNSIGNED,  MPI_COMM_WORLD);

    delete[] def_values;
    delete[] use_values;
    delete[] offset;
    delete[] length;
    delete[] offsetUse;
    delete[] lengthUse;
    def_values=NULL;
    use_values=NULL;
    offset=NULL;
    length=NULL;
    offsetUse=NULL;
    lengthUse=NULL;

    if (quickSave==false) {
      /* communicate all results */
      std::cerr << my_rank << " : communication done. Deserializing ..." << std::endl;

      MPI_Barrier(MPI_COMM_WORLD);
      if (my_rank==0) {
        Compass::gettime(e_time_node);
        double restime = Compass::timeDifference(e_time_node, b_time_node);
        std::cerr << "\n >>> communication (ARRAY) done. TIME : " << restime <<
          "   arrsize Def : " << global_arrsize << "  arrsize Use : " << global_arrsizeUse << std::endl;
      }



      /* deserialize all results */
      // write the global def_use_array back to the defmap (for each processor)
      ((DefUseAnalysis*)defuse)->flushDefuse();
      deserializeDefUseResults(global_arrsize, (DefUseAnalysis*)defuse, def_values_global, memTrav->nodeMap, true);
      deserializeDefUseResults(global_arrsizeUse, (DefUseAnalysis*)defuse, use_values_global, memTrav->nodeMap, false);
      std::cerr << my_rank << " : deserialization done." << std::endl;
      /* deserialize all results */


      MPI_Barrier(MPI_COMM_WORLD);
      if (my_rank==0) {
        Compass::gettime(end_time_node);
        double restime = Compass::timeDifference(end_time_node, begin_time_node);
        std::cerr << ">> ---- DefUse Analysis - time for ALL communication :  " << restime << " sec " << std::endl;
      }

      defmap = defuse->getDefMap();
      usemap = defuse->getUseMap();

      if (my_rank==0) {
        std::cerr <<  my_rank << ": Total number of def nodes: " << defmap.size() << std::endl;
        std::cerr <<  my_rank << ": Total number of use nodes: " << usemap.size() << std::endl << std::endl;
        //((DefUseAnalysis*)defuse)->printDefMap();
      }
    }
    //#endif
#else
    std::cerr << ">>>>>> running defuse analysis in SEQUENCE (NO MPI). "  << std::endl;
    defuse = new DefUseAnalysis(root);

    // tps: fixme (9 Jul 2008)
    // skipping defuse tests until they pass all compass tests
    ((DefUseAnalysis*)defuse)->run(false);
    std::cerr <<  "Total number of def nodes: " << defuse->getDefMap().size() << std::endl;
    std::cerr <<  "Total number of use nodes: " << defuse->getUseMap().size() << std::endl << std::endl;

#endif

  }

  ROSE_ASSERT(defuse);

      done = true;
    } //run(SgProject* root)

    PrerequisiteList getPrerequisites() const {
      return PrerequisiteList();
    }

    DefUseAnalysis* getSourceDefUse() const {
      ROSE_ASSERT (done);
      return defuse;
    }

}; //class SourceDefUsePrerequisite

extern SourceDefUsePrerequisite sourceDefUsePrerequisite;

#endif
