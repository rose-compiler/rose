#include "rose.h"
#include "autoTuningSupport.h"
#include "CommandOptions.h"
#include "OmpAttribute.h"
#include <set>

using namespace std;
using namespace GenUtil;
using namespace RoseHPCT;
using namespace Outliner;

namespace autoTuning
{
  bool aggressive_triage;
  bool enable_debug;
  float triage_threshold = 0.8; // default value, can be changed by a command option

  void autotuning_command_processing(vector<string>&argvList)
  {
    if (CommandlineProcessing::isOption (argvList,"-rose:autotuning:","enable_debug",true))
    {
      cout<<"Enabling debugging mode for auto tuning..."<<endl;
      enable_debug= true;
    }
    else
      enable_debug= false;

    if (CommandlineProcessing::isOptionWithParameter(argvList,"-rose:autotuning:","triage_threshold",triage_threshold, true))
    {
      if (autoTuning::enable_debug)
        cout<<"Setting custom triage threshold to ..."<<triage_threshold<<endl;
    }

    if (CommandlineProcessing::isOption (argvList,"-rose:autotuning:","aggressive_triage",true))
    {
      if (autoTuning::enable_debug)
        cout<<"Enabling aggressive code triage for auto tuning..."<<endl;
      aggressive_triage= true;
    }
    else
      aggressive_triage= false;


    // keep --help option after processing, let other modules respond also
    if ((CommandlineProcessing::isOption (argvList,"--help","",false)) ||
        (CommandlineProcessing::isOption (argvList,"-help","",false)))
    {       
      cout<<"Autotuning-specific options"<<endl;
      cout<<"\t-rose:autotuning:enable_debug               run autotuing in debugging mode"<<endl;
      cout<<"\t-rose:autotuning:triage_threshold  val      set triage threshold, val can be 0.6, 0.9,etc."<<endl;
      cout<<"\t-rose:autotuning:aggressive_triage          enable aggressive code triage"<<endl;
      cout <<"---------------------------------------------------------------"<<endl;     
    }

  }

  /*!
   *  \brief Checks if a given Sage node already has a metric attribute
   *  with the specified name and returns a pointer to this attribute if
   *  so, or NULL otherwise.
   */
  RoseHPCT::MetricAttr *
    checkMetricAttribute (SgNode* node, const std::string& metric_name)
    {
      if (node->attributeExists (metric_name))
      {
        AstAttribute* raw_attr = node->getAttribute (metric_name);
        return dynamic_cast<MetricAttr *> (raw_attr);
      }
      return NULL;
    }

  //! Generate a file metrics map
  std::map<string, std::map< std::string, double > >
    generateFileMetricsMap(std::set<const RoseHPCT::IRNode *> profFileNodes)
    {
      //ROSE_ASSERT( profFileNodes.size()>0);
      // convert file profile file nodes into a map for quick information retrieval 
      std::map<string, std::map< std::string, double > > fileMetrics;
      std::set<const RoseHPCT::IRNode *>::iterator iter= profFileNodes.begin();
      for (;iter!=profFileNodes.end(); iter++)
      {
        const RoseHPCT::IRNode * hpct_node = *iter;
        std::string file_name = hpct_node->getName();
        //cout<<"debug: file name="<< file_name<<endl;
        RoseHPCT::Observable::ConstMetricIterator m = hpct_node->beginMetric();
        for (;m!=hpct_node->endMetric();++m)
        {
          std::string metric_name= m->getName();
          double metric_value = m->getValue();
          fileMetrics[file_name][metric_name]=metric_value;
          //cout<<"debug fileMetrics: metric name="<< metric_name <<" value="<<metric_value<<endl;
        }
      }
      return fileMetrics;
    }

  //! Find the name of the time-consuming source file given a set of profile file nodes
  std::string findHottestFile(std::map<string, std::map< std::string, double > > fileMetrics ) 
  {
    // find the hottest file name
    // TODO what if users use PAPI_TOT_CYC ?
    std::string hot_file; 
    double greatest_value=0;
    std::map<string, std::map< std::string, double > > ::const_iterator map_iter;
    for (map_iter=fileMetrics.begin();map_iter!=fileMetrics.end();map_iter++)
    {
      std::string current_file=(*map_iter).first;
      if (fileMetrics[current_file]["WALLCLK"]>greatest_value)
      {
        hot_file= current_file;
        greatest_value = fileMetrics[current_file]["WALLCLK"];
      }
    }  
    //cout<<"debug in main: hot file="<< hot_file<<" value="<<greatest_value<<endl;
    return hot_file;
  }

  //! Collect available metric names
  std::vector <std::string>  collectMetricNames(std::map<std::string, std::map< std::string, double > > fileMetrics)
  {
    bool hasOrigFileMetrics = false;
    if (fileMetrics.size()>0) 
      hasOrigFileMetrics = true;

    //Collect used metric names for later use
    std::vector <std::string> metricNames;
    std::map<std::string, double >:: const_iterator metric_iter;
    int counter ; 
    if (hasOrigFileMetrics)
    { // automatically get available metrics
      std::string first_file = (*fileMetrics.begin()).first;
      counter= fileMetrics[first_file].size();
      //cout<<"debug in main: number of metrics="<< counter<<endl;
      for (metric_iter=fileMetrics[first_file].begin(); 
          metric_iter!=fileMetrics[first_file].end(); metric_iter++)
        metricNames.push_back((*metric_iter).first);
      //cout<<"debug in main: metric name 1="<< metricNames[0]<<endl;
    }
    else
    { // manually insert metric names otherwise
      // TODO add more PAPI metrics when needed
      metricNames.push_back("WALLCLK");
      metricNames.push_back("PAPI_TOT_CYC");
    }
    return metricNames;
  }

  //! Generate a node-metrics map for SgLocatedNode within a SgFile
  std::map<SgNode*, std::map< std::string, double > > 
    generateNodeMetricsMap(SgFile* file, std::vector <std::string > metricNames)
    {
      // generate a map of map
      std::map<SgNode*, std::map< std::string, double > >  nodesWithMetrics;
      // for each located node, check for each type of used metrics.
      Rose_STL_Container <SgNode*> located_nodes = NodeQuery::querySubTree(file, V_SgLocatedNode);
      Rose_STL_Container <SgNode*>::const_iterator node_iter = located_nodes.begin(); 
      for(;  node_iter!=located_nodes.end(); node_iter++)
      { 
        int limit= metricNames.size();
        for (int i=0;i<limit; i++)
        {
          MetricAttr* raw_attr= checkMetricAttribute (*node_iter,metricNames[i]);
          if (raw_attr)
          {
            nodesWithMetrics[*node_iter][metricNames[i]]=raw_attr->getValue();
            ROSE_ASSERT(raw_attr->getName() == metricNames[i]); // validate the metric name
          }
        }
      }

      //dump all WALLCLK value
      // std::map<SgNode*, std::map< std::string, double > >::const_iterator map_iter;
      // for (map_iter=nodesWithMetrics.begin();map_iter!=nodesWithMetrics.end();map_iter++)
      //   cout<<"dump metrics:"<<nodesWithMetrics[(*map_iter).first]["WALLCLK"]<<endl;
      return nodesWithMetrics;   
    }

  //! Find the hottest non-scope statement  
  SgStatement* findHottestStatement(SgFile* file, std::map<SgNode*, std::map< std::string, double > > nodeMetricsMap)
  {
    Rose_STL_Container <SgNode*> node_list= NodeQuery::querySubTree(file, V_SgStatement);
    Rose_STL_Container <SgNode*>::iterator node_iter = node_list.begin();
    SgNode* hot_node=NULL;
    //double greatest_value;
    double greatest_value = 0;
    for (; node_iter!=node_list.end(); node_iter++)
    { // only consider single-line statements for now
      if ((!isSgScopeStatement(*node_iter)) && (!isSgFunctionDeclaration(*node_iter)))
      {
        double cur_value = nodeMetricsMap[*node_iter]["WALLCLK"];
        if (cur_value> greatest_value)
        {
          greatest_value = cur_value;
          hot_node = *node_iter;
        }
      }
    }
    // cout<<"hot SgNode is:"<<hot_node->unparseToString()<<endl;
    // cout<<"hot value is:"<<greatest_value<<endl;
    return isSgStatement(hot_node);
  }

  //! Find a target loop from a hot statement
  SgForStatement* findTargetLoop(SgNode* hot_node)
  {
    // TODO handle Fortran
    SgForStatement* innermost_loop = isSgForStatement(SageInterface::findEnclosingLoop(isSgStatement(hot_node)));
    if (innermost_loop==NULL) 
      return NULL;
    SgForStatement* outermost_loop= innermost_loop;

    //find the outermost parallelizable loop as the target loop
    // use OpenMP for pragma as hints if available
    if (aggressive_triage) 
    {
      std::vector<SgForStatement*> forLoopVec;
      forLoopVec.push_back(innermost_loop);
      SgNode* p = innermost_loop->get_parent();
      ROSE_ASSERT(p!=NULL);
      while (isSgGlobal(p)==NULL)
      {
        if (isSgForStatement(p) )
          forLoopVec.push_back(isSgForStatement(p));
        p = p->get_parent();
        ROSE_ASSERT(p!=NULL);
      }

      // reverse iterate the loop vector
      std::vector<SgForStatement*>::reverse_iterator iter;
      for (iter = forLoopVec.rbegin(); iter != forLoopVec.rend(); iter ++)
      {
        OmpSupport::OmpAttribute* attribute = OmpSupport::getOmpAttribute(*iter);
        if (attribute)
        {  // Could be either #pragma omp for or #pragma omp parallel for
          if ((attribute->getOmpDirectiveType()==OmpSupport::e_for)||
              (attribute->getOmpDirectiveType()==OmpSupport::e_parallel_for))
          {
            if (enable_debug)
              cout<<"Found OMP attribute attached on loop at line:"<<(*iter)->get_file_info()->get_line()<<endl;
            outermost_loop = *iter;
            break;
          }
        }
      } 
#if 0      
      SgForStatement* forloop = isSgForStatement(SageInterface::getEnclosingNode<SgForStatement>(outermost_loop));
      if (forloop != NULL )
        outermost_loop = forloop;
#endif        
    } else 
      // find the innermost, consecutive (may have bare SgBasicBlocks in between), 
      // enclosing loops of the hot statement
    {
      SgNode* cur_node= innermost_loop;
      SgNode* par_node = innermost_loop->get_parent();
      while (isSgForStatement(par_node)||isSgBasicBlock(par_node))
      {
        // skip BB containing only a loop
        SgBasicBlock * bb = isSgBasicBlock(par_node);
        if (bb) 
        { 
          SgStatementPtrList stmtlist = bb->get_statements();
          //TODO there should be configurable levels of aggressiveness to choose the target loop
          // strictest: contains only the current loop  //    (stmtlist.size()>1))
          // or loop is the first within the BB (stmtlist[0]!=cur_node)
          // or loop is the last within the BB  (stmtlist[stmtlist.size()-1]!=cur_node))
          if (stmtlist[0]!=cur_node)
            break; // terminate the search
        }
        else
          outermost_loop = isSgForStatement(par_node);
        cur_node= par_node;
        par_node = cur_node->get_parent();
      } // end while
      //cout<<"hot target loop is:"<<outermost_loop->unparseToString()<<endl;
    }
    return outermost_loop;
  }

  //!Customized comparison function to sort profile IR node with percentage metrics.
  // sort based on percentage of exclusive wall clock
  static bool compareProfNode(const RoseHPCT::IRNode* s1, const RoseHPCT::IRNode* s2)
  {
    double v1, v2;
    v1 = s1->getMetricValue(m_percentage);
    v2 = s2->getMetricValue(m_percentage);
    return v1>v2; // we need decremental order:the biggest is put the first
  }

/*!
 *
 * This is the code triage module for the empirical tuning framework.
 *
 * It is based on the percentage of execution time of each non-scope statement.
 * A threshold is predefined, like 80%:  top hot statements account for at least 80% of execution time
 * will be specified as autotuning candidates.
 *
 * Depending on different triage policies, several corresponding targets, mostly loops,
 * are identified as tuning targets and outlined.
 * It is possible two hot statements are enclosed within one target loop.
 *
 */
  void code_triage(std::set<SgForStatement*>& candidateSgLoops)
  {
    // -------------------------------------------------
    // Find enough profile IR statement nodes exceeding a threshold for execution percentage
    //
    //x Sort profiling IR's statement nodes by percentage: descending order
    //  Have to transfer them into a container supporting random access in order to use std::sort()
    std::vector <const RoseHPCT::IRNode *> nodeVec, candidateVec;
    std::set<const RoseHPCT::IRNode *>:: const_iterator piter= RoseHPCT::profStmtNodes_.begin();
    for (;piter!=RoseHPCT::profStmtNodes_.end();piter++)
    {
      const RoseHPCT::IRNode * node = *piter;
      nodeVec.push_back(node);
    }
    // random access iterator is needed
    sort (nodeVec.begin(), nodeVec.end(), compareProfNode);
    // x. Find enough hot statements until a threshold is just exceeded.
    double sum =0.0;
    for ( std::vector <const RoseHPCT::IRNode *>::const_iterator iter = nodeVec.begin(); iter!=nodeVec.end(); iter++)
    {
      //const RoseHPCT::IRNode * node = *iter;
      sum+= (*iter)->getMetricValue(m_percentage);
      if (autoTuning::enable_debug)
        cout<<"Candidate statement node:"<<(*iter)->toString()<<" "<<"sum="<<sum<<endl;
      // save stmt nodes until threshold is reached.
      ROSE_ASSERT(autoTuning::triage_threshold>0.5);
      candidateVec.push_back(*iter);
      if (sum>autoTuning::triage_threshold)
        break;
    }
    // Find relevant SgNode matching the candidate profile statement IR nodes
    // -------------------------------------------------
    std::vector <SgNode* > candidateSgNodes;
    for ( std::vector <const RoseHPCT::IRNode *>::const_iterator iter = candidateVec.begin(); iter!=candidateVec.end(); iter++)
    {
      std::set<SgLocatedNode *> matched_set =  RoseHPCT::profSageMap_[*iter];
      RoseHPCT::IRNode * irnode = const_cast<RoseHPCT::IRNode *> (*iter);
      // each candidate Profile IR node should have at least one matching ROSE AST node
      // if the SgProject has a SgSourceFile matching the profile node's file information
      if (matched_set.size()==0)
      {
        RoseHPCT::Located * l = dynamic_cast<RoseHPCT::Located *> (irnode);
        ROSE_ASSERT(l);
        SgFilePtrList file_pointer_list = SageInterface::getProject()->get_fileList();
        SgFilePtrList::iterator file_iter = file_pointer_list.begin();
        for (;file_iter!=file_pointer_list.end();file_iter++)
        {
          SgFile* file= *file_iter;
          if (isSgSourceFile(file))
          {
            string file_name = isSgSourceFile(file)->get_file_info()->get_filename();
            if (file_name == l->getFileNode()->getName())
            {
              cerr<<"Fatal error: cannot find a matching SgNode for a profile IR node:"<<(*iter)->toString()<<endl;
              ROSE_ASSERT(false);
            }
          }
        }
      }
      //std::copy(matched_set.begin(), matched_set.end(), back_inserter(candidateSgNodes));
      for (std::set<SgLocatedNode *>::const_iterator src=matched_set.begin(); src!=matched_set.end(); src++)
      {
        //          cout<<"Found a match SgNode:"<<(*src)->unparseToString()<<
        //          "\n for prof Node:\n"<<(*iter)->toString()<<endl;
        candidateSgNodes.push_back(*src);
      }
    }
    // Find auto tuning targets based on candidate Sage statement nodes
    // -------------------------------------------------
    //  A std::set is used to insure only unique loops are kept
    //  since different hot statements may lead to a same target loop
    //std::set<SgForStatement*> candidateSgLoops;
    if (autoTuning::enable_debug)
      cout<<"Candidate SgNode count="<<candidateSgNodes.size()<<endl;
    for (std::vector <SgNode* >::const_iterator iter = candidateSgNodes.begin();
        iter!=candidateSgNodes.end(); iter++)
    {
      SgNode* hot_node = *iter;
      if (autoTuning::enable_debug)
        cout<<hot_node->class_name()<<" "<<hot_node->unparseToString()<<endl;
      SgForStatement* outermost_loop= findTargetLoop(hot_node);
      if (outermost_loop!=NULL)
      {
        if (isOutlineable (outermost_loop))
        {
          candidateSgLoops.insert(outermost_loop);
        }
        else
        {
          cerr<<"Warning: skip a target loop at line "<<outermost_loop->get_file_info()->get_line()<<" is not outlineable"<<endl;
        }
      }
      else
      {
        cerr<<"Warning: no handling for hot statements outside a for loop"<<endl;
        cerr<<hot_node->unparseToString()<<endl;
      }
    }

#if 0
    // Old Code triage:
    // -----------------------------------------------------
    // This step is better done after the file level performance metrics
    // are generated, either by ROSE's propagation or by HPCToolKit's correlation command.
    // Alternative: bottom-up method: find hot statements, back track to their file names

    // find the hottest file name
    std::map<string, std::map< std::string, double > >
      fileMetrics = generateFileMetricsMap(RoseHPCT::profFileNodes_);
    std::string hot_file = findHottestFile(fileMetrics);
    bool hasOrigFileMetrics = false;
    if (fileMetrics.size()>0)
      hasOrigFileMetrics = true;

    //Collect used metric names for later use
    std::vector <std::string> metricNames = collectMetricNames(fileMetrics);
    // We don't use global analysis with AST merge,
    // So a file name matching is necessary here.
    SgFilePtrList file_pointer_list = project->get_fileList();
    SgFilePtrList::iterator file_iter = file_pointer_list.begin();
    for (;file_iter!=file_pointer_list.end();file_iter++)
    {
      SgFile* file= *file_iter;
      if (!isSgSourceFile(file))
        continue;
      // Does the current file math the file containing the target hot statement?
      if (hasOrigFileMetrics&&
          file->get_file_info()->get_filenameString() != hot_file)
        continue;
      // cout<<"Found a matching file!"<<endl;

      // generate a map of map
      std::map<SgNode*, std::map< std::string, double > >
        nodesWithMetrics = generateNodeMetricsMap(file,metricNames);
      // Find hottest statement then backtrack to hottest loops
      // This makes sense since HPCToolkit generates accumulative flat profiling metrics for each statements,
      // Metrics of statements within loops already consider loop iterations (and function calls).
      SgNode* hot_node=findHottestStatement(file, nodesWithMetrics);
      if (hot_node==NULL)
      {
        cerr<<"No hot node is found."<<endl;
        continue;
      }

      // find the innermost, consecutive (may have bare SgBasicBlocks in between),
      // enclosing loops of the hot statement
      // TODO handle Fortran
      SgForStatement* outermost_loop= findTargetLoop(hot_node);
      if (outermost_loop!=NULL)
      {
        // outline the target loop
        if (isOutlineable (outermost_loop))
          outline(outermost_loop);
        else
        {
          cerr<<"Target loop at line "<<outermost_loop->get_file_info()->get_line()<<" is not outlineable, abort!"<<endl;
          ROSE_ASSERT(false);
        }
      }
      else
      {
        cerr<<"Warning: no handling for hot statements outside a for loop"<<endl;
        cerr<<hot_node->unparseToString()<<endl;
      }
    } // end for each file
#endif

  }

} // end namespace

