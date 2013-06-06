#include <boost/pending/disjoint_sets.hpp>
#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <iostream>
#include <fstream>
#include "sqlite3x.h"
#include <list>
#include <math.h>
#include <boost/program_options.hpp>
#include <algorithm>
#include <string>
#include <cstdio> // for std::remove
#include "lsh.h"
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace sqlite3x;
using namespace boost::program_options;

#define PRINTEDGELABEL 0
#define FILTER 1
#define VIZZ3D 0
#define FILTERCLUSTER2NODES 1

struct Element {
  int cluster;
  int64_t function_id;
  std::string file;
  std::string function_name;
  int64_t begin_index;
  int64_t end_index;
  int64_t size;
};

struct Node_type  {
  int nr;
  int size;
  std::string name;
  std::string color;
  bool merged;
  std::string clusterPrintName;
};

double str2int (const string &str) {
  stringstream ss(str);
  double n;
  ss >> n;
  return n;
}

string int2str (double n) {
  stringstream ss;
  ss << n;
  return ss.str();
}

string int2str (int n) {
  stringstream ss;
  ss << n;
  return ss.str();
}


std::string getPathColor(std::string path) {
  string nodeColor="BBBBBB"; // grey
  if (path.find("./usr")!=std::string::npos) nodeColor="FF0000";

  if (path.find("./usr/lib")!=std::string::npos) nodeColor="FF9900"; //orange
  if (path.find("./lib")!=std::string::npos) nodeColor="FFFF66"; //yellow

  if (path.find("./usr/bin")!=std::string::npos) nodeColor="0000FF"; //blue
  if (path.find("./usr/sbin")!=std::string::npos) nodeColor="0000FF"; //blue
  if (path.find("./bin")!=std::string::npos) nodeColor="00FFFF"; //lightblue
  if (path.find("./sbin")!=std::string::npos) nodeColor="00FFFF"; //lightblue

  if (path.find("./var")!=std::string::npos) nodeColor="9900FF"; //lila
  if (path.find("./etc")!=std::string::npos) nodeColor="9900FF"; //lila
  if (path.find("./sys")!=std::string::npos) nodeColor="9900FF"; //lila
  if (path.find("./boot")!=std::string::npos) nodeColor="9900FF"; //lil
  if (path.find("./dev")!=std::string::npos) nodeColor="9900FF"; //lila


  if (path.find("./Com")!=std::string::npos) nodeColor="FF0000";
  if (path.find("./drivers")!=std::string::npos) nodeColor="FFFF66"; //yellow
  if (path.find("./DirectX")!=std::string::npos) nodeColor="0000FF"; //blue
  if (path.find("./spool")!=std::string::npos) nodeColor="0000FF"; //blue
  if (path.find("./wbem")!=std::string::npos) nodeColor="FFFF66"; //yellow
  if (path.find("./usmt")!=std::string::npos) nodeColor="FF9966"; //orange
  if (path.find("./oobe")!=std::string::npos) nodeColor="00FFFF"; //lightblue
  if (path.find("./mui")!=std::string::npos) nodeColor="00FFFF"; //lightblue
  if (path.find("./Macromed")!=std::string::npos) nodeColor="9900FF"; //lila

  return nodeColor;
}

bool isContainedEdge(multimap<std::string, std::string>& edges, std::string name1, std::string name2) {
  bool found = false;
  multimap<string, string>::const_iterator low = edges.lower_bound(name1);
  multimap<string, string>::const_iterator up = edges.upper_bound(name1);
  for (; low !=up;++low) {
    if (low->second==name2)
      found = true;
  }
  return found;
}

int main(int argc, char* argv[])
{
  std::string database;

  try {
    options_description desc("Allowed options");
    desc.add_options()
      ("help", "produce a help message")
      ("database,q", value< string >()->composing(), 
       "the sqlite database that we are to use")
      ;

    variables_map vm;
    store(command_line_parser(argc, argv).options(desc)
	  .run(), vm);


    if (vm.count("help")) {
      cout << desc;            
      exit(0);
    }

    if (vm.count("database")!=1  ) {
      std::cerr << "Missing options. Call as: findClones --database <database-name>" 
		<< std::endl;
      exit(1);

    }

    database = vm["database"].as<string >();
    cout << "database: " << database << std::endl;


  }
  catch(std::exception& e) {
    cout << e.what() << "\n";
  }



  sqlite3_connection con;
  con.open(database.c_str());


  double similarity =1.000;
  
  int windowSize=0;
  int stride=0;
  
  try {
    similarity = sqlite3x::sqlite3_command(con, "select similarity_threshold from detection_parameters limit 1").executedouble();
  } catch (std::exception& ex) {std::cerr << "Exception Occurred: " << ex.what() << std::endl;}
  try {
    windowSize = sqlite3x::sqlite3_command(con, "select window_size from run_parameters limit 1").executeint();
  } catch (std::exception& ex) {std::cerr << "Exception Occurred: " << ex.what() << std::endl;}
  try {
    stride = sqlite3x::sqlite3_command(con, "select stride from run_parameters limit 1").executeint();
  } catch (std::exception& ex) {std::cerr << "Exception Occurred: " << ex.what() << std::endl;}


  std::cout << "Dataset generated with similarity " << similarity << " stride " << stride << " windowSize " << windowSize 
            << std::endl;




  scoped_array_with_size<vector<Element> > vectorOfClusters;

  int eltCount =0;



#if 0
  try {
    if( similarity == 1.0)
      eltCount = lexical_cast<size_t>(con.executestring("select count(distinct cluster) from postprocessed_clusters"));

    else
      eltCount = boost::lexical_cast<size_t>(con.executestring("select count(distinct cluster) from clusters"));
  } catch (exception& e) {cerr << "Exception: " << e.what() << endl;}
#endif
  
  try {
    eltCount = boost::lexical_cast<size_t>(con.executestring("select count(distinct cluster) from largest_clusters"));
  } catch (exception& e) {cerr << "Exception: " << e.what() << endl;}

  if (eltCount == 0) {
    cerr << "No largest_clones table found -- invalid database?" << endl;
    exit (1);
  }

  map<string,int> fileSizes;

  std::cout << "Allocating " << eltCount << " elements " << std::endl;
  vectorOfClusters.allocate(eltCount);
  //Create set of clone pairs
  try{
    std::string selectSeparateDatasets ="select c.cluster,  fid.row_number, fid.file, fid.function_name, c.begin_index_within_function, c.end_index_within_function  from "  
      " largest_clusters c join function_ids fid  on c.function_id = fid.row_number ORDER BY c.cluster;";

    sqlite3_command cmd(con, selectSeparateDatasets.c_str());
    sqlite3_reader datasets=cmd.executereader();

    // int64_t thisClusterName;
    
   
    std::vector<Element> thisCluster;
    
    //for all integers
    //    int set_number = ds.find_set(0);

    
    bool first = true;
    int64_t last_clone=0;
    int64_t clone_cluster=0;
      
    while(datasets.read())
      {
    
	Element cur_elem;
	cur_elem.cluster       = boost::lexical_cast<int64_t>(datasets.getstring(0));
	cur_elem.function_id   = boost::lexical_cast<int64_t>(datasets.getstring(1));
	cur_elem.file          = datasets.getstring(2);
	cur_elem.function_name = datasets.getstring(3);
	cur_elem.begin_index          = boost::lexical_cast<int64_t>(datasets.getstring(4));
	cur_elem.end_index        = boost::lexical_cast<int64_t>(datasets.getstring(5));
	cur_elem.size        = cur_elem.end_index-cur_elem.begin_index;

        if (first) {
          first = false;
          last_clone = cur_elem.cluster;
        }

	if( cur_elem.cluster != last_clone )
	  {
	    last_clone = cur_elem.cluster;
	    clone_cluster++;
	  }

	string nametail = boost::filesystem::path(cur_elem.file).leaf();
	if (nametail.rfind('-') != string::npos) { // Remove "expanded-" prefix
	  int pos = cur_elem.file.rfind('-');
	  cur_elem.file = cur_elem.file.substr(0, pos);
	}
      
	if(clone_cluster == eltCount)
	  {
	    std::cout << "Bad idea: eltCount exceeded " << std::endl;

	    exit(1);
	  }

	// only push back if different file name
	bool found=false;
	for(size_t j=0; j < vectorOfClusters[clone_cluster].size(); j++) {
	  if (vectorOfClusters[clone_cluster][j].file==cur_elem.file) {
	    found = true;
            fileSizes[cur_elem.file]++;
#if 0
	    map<string,int>::const_iterator it = fileSizes.find(cur_elem.file);
	    int size = 1;
	    if (it!=fileSizes.end())
	      size = it->second;
	    fileSizes.erase(cur_elem.file);
	    // remember the current file and its size (amount of functions contained)
	    fileSizes.insert(make_pair(cur_elem.file,size+1));
	    //	    cerr << " replacing " << cur_elem.file << " by size " << size << endl;
#endif
	  }
	}      
	if (!found) {
	  vectorOfClusters[clone_cluster].push_back(cur_elem);
	  fileSizes.insert(make_pair(cur_elem.file,1));
	}
      }
  }catch(exception &ex) {
    cerr << "Exception Occured: " << ex.what() << endl;
  }

  std::cerr << " Starting DB request ... " << endl;
  int filterMultiEdges=0;
  int filter2Nodes=0;
  map<std::string,Node_type> clusters;
  map<std::string,Node_type> files;
  multimap<int, int> edges;
  map<std::string,multimap<std::string, std::string> > nodeEdges;
  int counter=0;
  // iterate through all elements (nodes)
  cerr << " Nr of clusters : " << eltCount << endl;
  for(int i = 0 ; i < eltCount; i++)  {
    if ( vectorOfClusters[i].size()>=2) {
      // allow only clusters with at least 2 distinct elements
      std::cout << "Cluster " << vectorOfClusters[i][0].cluster <<
	" has " << vectorOfClusters[i].size() << " elements. " << std::endl;

      // ***********************************************************************
      // this part handles all clusters and files for cluster with more than 2 files 
      //      std::string clusterName = boost::lexical_cast<std::string>(vectorOfClusters[i][0].cluster);
      Node_type Cluster;
      Cluster.nr=counter;
      //Cluster.name=clusterName;
      Cluster.name="";
      Cluster.merged=false;
      Cluster.size=(vectorOfClusters[i].size())+20;
      // adjust cluster name
      for(size_t j=0; j < vectorOfClusters[i].size(); j++) {
	string fileName = vectorOfClusters[i][j].file;
	Cluster.name+="-"+fileName;//int2str(vectorOfClusters[i][0].cluster);//
      } 
      Cluster.clusterPrintName="";
      // see if such a cluster exists and get it if so
      map<std::string,Node_type>::const_iterator it = clusters.find(Cluster.name);
      if (it!=clusters.end())
	Cluster = it->second;
      

      // i iterates through all clones, j iterates through all files in a clone
      for(size_t j=0; j < vectorOfClusters[i].size(); j++) {
	// check if files contains the current node, if not, create it
	map<std::string,Node_type>::const_iterator fIt = files.find(vectorOfClusters[i][j].file);
	if (fIt==files.end()) {
	  // create new Node
	  Node_type File;
	  File.nr= counter+j+1;
	  File.size=1;
	  File.name=vectorOfClusters[i][j].file;
	  string color = getPathColor(File.name);
	  //cerr << " color = " << color << " for " << File.name << endl;
	  File.color=color;
	  // find the size of the file in fileSizes
	  map<string,int>::const_iterator it = fileSizes.find(vectorOfClusters[i][j].file);
	  if (it!=fileSizes.end())
	    File.size = it->second;
	  // add a new file and its size into the vector (since not present)
	  files.insert(make_pair(vectorOfClusters[i][j].file, File));
	  //  create new edge between current cluster (counter) and node
	  edges.insert(make_pair(Cluster.nr,File.nr));
	  // this is a map between a node and its edges
	  // never been to this node before, create edge for this node
	  nodeEdges[File.name].insert(make_pair(File.name,Cluster.name));
	  nodeEdges[Cluster.name].insert(make_pair(Cluster.name,File.name));
	}  else {
	  // node already exists, create only an edge
	  Node_type node = fIt->second;
	  int pre_count = node.nr;
	  // the cluster might exist already, check if edge already exists
	  bool found = false;
	  multimap<int, int>::const_iterator low = edges.lower_bound(Cluster.nr);
	  multimap<int, int>::const_iterator up = edges.upper_bound(Cluster.nr);
	  for (low; low !=up;++low) {
	    if (low->second==node.nr) {
	      found = true;
	      filterMultiEdges++;
	    }
	  }
	  if (!found) {
	    edges.insert(make_pair(Cluster.nr,pre_count));
	    // we have seen this node before, we want to make sure we do not add another similar edge
	    nodeEdges[node.name].insert(make_pair(node.name,Cluster.name));
	    nodeEdges[Cluster.name].insert(make_pair(Cluster.name,node.name));
	  }
	}

	std::cout << "  elem " << j 
		  << " function id " << vectorOfClusters[i][j].function_id 
		  << " file " << vectorOfClusters[i][j].file
		  << " function name " << vectorOfClusters[i][j].function_name
		  << " begin address " << hex << vectorOfClusters[i][j].begin_index 
		  << " end address " << hex << vectorOfClusters[i][j].end_index
		  << dec << std::endl;
           
      }
      // add all clusters into clusters map
      //cerr << " inserting cluster : " << Cluster.name << " ClusterNr " << Cluster.nr << endl;
      Cluster.color ="009900";
      clusters.insert(make_pair(Cluster.name,Cluster));
      counter+=vectorOfClusters[i].size();
      counter++;
      //	if (counter>=300) break;
    }
  }
  // ***********************************************************************




  // ***********************************************************************
  // iterate through all nodes and check if only 1 parent exists.
  // If true, aggregate
  int nodesAggregated=0;
  cerr << "Writing to output.gml file ... " << endl;
#if FILTER
  map<std::string,multimap<std::string, std::string> >::const_iterator fit = nodeEdges.begin();
  for (;fit!=nodeEdges.end();++fit) {
    std::string key = fit->first;
    multimap<std::string, std::string> edge = fit->second;
    //cerr << " node : " << key << "  has " << edge.size() << " edges." << endl;
    if (edge.size()==1) {
      multimap<std::string, std::string>::const_iterator low = edge.lower_bound(key);
      multimap<std::string, std::string>::const_iterator up = edge.upper_bound(key);
      for (low; low !=up;++low) {
	std::string sourceName = low->first; // file
	std::string targetName = low->second; // cluster
	// if a node has only 1 edge, aggregate.

	map<std::string,Node_type>::const_iterator fItS = files.find(sourceName);
	if (fItS!=files.end()) {
	  // remove source , which is the file
	  Node_type source=fItS->second;
	  source.color="66FFFF"; // light blue
	  files.erase(sourceName);
	  nodesAggregated++;
	  // insert this if needed for visualization (to see what is going to be deleted!!)
	  //files.insert(make_pair(sourceName,source));
	}
	//	cerr << " searching for target name (in clusters) :   " << targetName << endl;
	// search for the target in case it is a file
	map<std::string,Node_type>::const_iterator fIt = clusters.find(targetName);
	if (fIt!=clusters.end()) {
	  // keep target, which is the cluster
	  Node_type target=fIt->second;
	  target.color="66FF66";
	  target.merged=true;
	  target.clusterPrintName+=sourceName;
	  //cerr << " trying to color the clone light green  " << target.name << endl;
	  clusters.erase(target.name);
	  clusters.insert(make_pair(target.name,target));
	}

      }
    }
  }
#endif    
  cerr << "     Aggregated 1nodes (merged leaves to cluster) : "<< nodesAggregated << " nodes. " << endl;
  // ***********************************************************************




  // ***********************************************************************
  // Filter away all clusters that have 2 nodes
  // If true, aggregate
  int removedNodes=0;
#if FILTERCLUSTER2NODES
  fit = nodeEdges.begin();
  for (;fit!=nodeEdges.end();++fit) {
    std::string key = fit->first;
    multimap<std::string, std::string> edge = fit->second;
    //cerr << " node : " << key << "  has " << edge.size() << " edges." << endl;
    if (edge.size()==2) { //we look for clusters with 2 outedges
      map<std::string,Node_type>::const_iterator it = clusters.find(key);
      if (it!=clusters.end()) {
        removedNodes++;
	//cerr << "\nFOUND a cluster with 2 outedges " << key << endl;
	Node_type sourceE;
	Node_type targetE;
	int count=0;
	bool mergedNode=false;
	multimap<std::string, std::string>::const_iterator lower = edge.lower_bound(key);
	multimap<std::string, std::string>::const_iterator uper = edge.upper_bound(key);
	for (lower; lower !=uper;++lower) {
	  std::string sourceName = lower->first; // file
	  std::string targetName = lower->second; // cluster
	  // the first node is a cluster (source) the second the file
	  //cerr << " edge  " << count << " : " << sourceName << " -> " << targetName << "    found." << endl;
	  Node_type source;
	  Node_type target;
	  map<std::string,Node_type>::const_iterator fItS = clusters.find(sourceName);
	  if (fItS!=clusters.end()) {
	    // remove source , which is the cluster
	    source=fItS->second;
	    if (source.merged==true) {
	      // this node is skipped since this cluster node has been aggregated before and does not need to be removed.
	      mergedNode=true;
	      continue;
	    }
	    source.color="66FFFF"; // light blue
	    clusters.erase(sourceName);
	    // insert this if needed for visualization (to see what is going to be deleted!!)
	    //clusters.insert(make_pair(sourceName,source));
	  }
	  //	cerr << " searching for target name (in files) :   " << targetName << endl;
	  // search for the target in case it is a file
	  map<std::string,Node_type>::const_iterator fIt = files.find(targetName);
	  if (fIt!=files.end()) {
	    // keep target, which is the cluster
	    target=fIt->second;
	    //target.color="660000";
	    //cerr << " trying to color the clone light green  " << target.name << endl;
	    files.erase(target.name);
	    files.insert(make_pair(target.name,target));
	    if (count==0) sourceE=target; else targetE=target;
	    count++;
	  }
	  // erase edges between cluster and file
	  //cerr << "   Erasing edge " << " " << source.nr << " -> " << target.nr << endl;
	  multimap<int, int>::iterator lowIt = edges.lower_bound(source.nr);
	  multimap<int, int>::iterator upIt = edges.upper_bound(source.nr);
	  for (lowIt; lowIt !=upIt;++lowIt) {
	    if (lowIt->second==target.nr) {
	      edges.erase(lowIt);
	      break;
	    }
	  }
	} // for lower
	if (mergedNode==false) {
	  //cerr << "   Creating edge " << " " << sourceE.nr << " -> " << targetE.nr << endl;
	  // this code creates new edges between 2 nodes directly (skipping cluster)
	  bool foundS = false;
	  multimap<int, int>::const_iterator low = edges.lower_bound(sourceE.nr);
	  multimap<int, int>::const_iterator up = edges.upper_bound(sourceE.nr);
	  for (low; low !=up;++low) {
	    if (low->second==targetE.nr)
	      foundS = true;
	  }
	  bool foundT = false;
	  low = edges.lower_bound(targetE.nr);
	  up = edges.upper_bound(targetE.nr);
	  for (low; low !=up;++low) {
	    if (low->second==sourceE.nr)
	      foundT = true;
	  }
	  if (foundS==false && foundT==false)
	    edges.insert(make_pair(sourceE.nr,targetE.nr));
	}
      } // if !=cluster.end

    } // if edges.size==2
  }
#endif    
  cerr <<  "     Aggregated 2nodes (removed cluster with 2 nodes) : "<< removedNodes << " nodes. " << endl;
  nodesAggregated+=removedNodes;
  // ***********************************************************************




  // ***********************************************************************
  // write to file
  std::string filename = "output.gml";
  std::ofstream myfile;
  myfile.open(filename.c_str());
  myfile << "graph  [\ndirected 0\n" <<endl;

  set<int> all_nodes;

  double min = 0.5;

  map<std::string, Node_type>::const_iterator vec=clusters.begin();
  for (;vec!=clusters.end();++vec) {
    Node_type cluster = vec->second;
    std::string vecName = cluster.name;
    int nr = cluster.nr;
    all_nodes.insert(nr);
    int len = cluster.size;
    int fac = 3;
    string color = cluster.color;
    int w = len;
    myfile << "  node [ id " << nr << endl;
    if (cluster.merged) {
      myfile << "    label \"" << cluster.clusterPrintName <<"\""; //<< endl 
      w = (cluster.clusterPrintName.size())*7;    
    } else {
      myfile << "    label \"" << int2str(nr) <<"\""; //<< endl 
    }
#if VIZZ3D
    myfile << "    Node_Color_ \"" << color << "\"" //<< endl
	   << "    Node_Shape_ \"0\"" //<< endl
	   << "    Width_ \""<< (log(len/fac+min)) << "\"" //<< endl
	   << "    Height_ \""<< (log(len/fac+min)) << "\"" //<< endl
	   << "    Depth_ \""<< (log(len/fac+min)) << "\"" //<< endl
	   << "    Type_ \"[ 67108864 FUNCTION_NODE ]\"" << endl;
#endif
    myfile << "    graphics [ type \"ellipse\" fill \"#"<<color<<"\" w " << w << " h " << len << " ]" //<< endl
	   << "  ] " << endl;
  }

  vec=files.begin();
  for (;vec!=files.end();++vec) {
    Node_type node = vec->second;
    std::string vecName = vec->first;
    int nr = node.nr;
    all_nodes.insert(nr);
    double len = (double)node.size;
    string color = node.color;
    int w = (vecName.size())*7;    
    if (len<=0) len =0.5;
    int h = (int)(15*log(len)+15);

    int fac = 3;
    myfile << "  node [ id " << nr << endl 
	   << "    label \"" << vecName <<"\""; //<< endl 
#if VIZZ3D
    myfile << "    Node_Color_ \""<< color << "\"" //<< endl
	   << "    Node_Shape_ \"0\"" //<< endl
	   << "    Width_ \""<< (log(w/fac+min)) << "\"" //<< endl
	   << "    Height_ \""<< (log(h/fac+min)) << "\"" //<< endl
	   << "    Depth_ \""<< (log(len/fac+min)) << "\"" //<< endl
	   << "    Type_ \"[ 67108864 FILE_NODE ]\"" << endl;
#endif
    myfile << "    graphics [ type \"rectangle\" fill \"#" << color << "\" w "<<w<<" h "<<h<<" ]" //<< endl
	   << "  ] " << endl;
  }

  int skippedEdge=0;
  map<int,int>::const_iterator edgeIt = edges.begin();
  for (;edgeIt!=edges.end();++edgeIt) {
    int from = edgeIt->first;
    int to = edgeIt->second;
    // make sure from and to exist!!
    set<int>::const_iterator itF = all_nodes.find(from);
    set<int>::const_iterator itT = all_nodes.find(to);
    if (itF==all_nodes.end() || itT==all_nodes.end()) {
      //cerr << "Skipping edge ... not found! " << endl;
      skippedEdge++;
      continue;
    }

    string edgeColor = "FFFFCC";
    myfile << "  edge [ "
#if PRINTEDGELABEL
	   << "         label \"" << from << "->" << to <<"\"" ;
#endif
    << "   source " << from << " target " << to; //<< endl
#if VIZZ3D
    //     << "         Multiplicity_ \""<<count<<"\"" << endl
    //     << "         Edge_Radius_ \""<<count<<"\"" << endl
    myfile << "    Edge_Color_ \"" << edgeColor << "\""; //<< endl
#endif
    myfile << "  ] " << endl;
  }

  myfile << "\n]\n" << endl;
  myfile.close();

  int  totalbefore = clusters.size()+filter2Nodes+ files.size()+nodesAggregated+ edges.size()+filterMultiEdges;
  int  totalafter  = clusters.size()+ files.size()+ edges.size()-skippedEdge;
  std::cerr << "Number of original clusters : " << (clusters.size()+filter2Nodes) <<  
    "  original number of files: " << (files.size()+nodesAggregated) << "    original nr edges : " << (edges.size()+filterMultiEdges) << 
    "  TOTAL OBJECTS: " << totalbefore << endl;
  std::cerr << "Number of clusters : " << clusters.size() << "  Number of files: " << files.size() <<
    " Number of edges : " << (edges.size()-skippedEdge) <<  
    "  TOTAL AFTER : " << totalafter << endl; 
  
  std::cout << "\n\nDataset generated with similarity " << similarity << " stride " << stride << " windowSize " << windowSize 
            << std::endl;

  // ***********************************************************************

  if( similarity == 1.0)
    std::cout << "\n\nClones are from the postprocessed dataset since the data was generated using exact clone detection " << std::endl;
  else
    std::cout << "\n\nClones are from the non-postprocessed dataset since the data was generated using inexact clone detection " << std::endl;

  return 0;
};



