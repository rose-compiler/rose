#include <math.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <iostream>
#include <fstream>
#include "sqlite3x.h"
#include <boost/program_options.hpp>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <string>
#include <cstdio> // for std::remove
#include <istream>
using namespace std;
using namespace sqlite3x;
using namespace boost::program_options;

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

// If the file has already been created as a node, get its ID.
// If not, create an ID for it and output its node declaration to myfile.
size_t createOrGetFileID(ostream& myfile, const string& filename, const string& filesize) {
  static map<string, size_t> fileIDs;
  string path;
  map<string, size_t>::const_iterator i = fileIDs.find(filename);
  if (i == fileIDs.end()) {
    size_t id = fileIDs.size();
    fileIDs.insert(std::make_pair(filename, id));
    path = boost::filesystem::path(filename).remove_leaf().string();
    string nametail = boost::filesystem::path(filename).leaf();
    cerr << " filename  " << filename << " tail : " << nametail << endl;

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

    double size=str2int(filesize);
    size = 0.15+log(1+size)*0.03;
    if (size<0.15) size =0.15;
    string size_s = int2str(size);
    if (nametail.rfind('-') != string::npos) { // Remove "expanded-" prefix
      int pos = nametail.rfind('-');
      nametail = nametail.substr(0, pos);
    }
    myfile << "  node [ id " << id << endl 
	   << "         label \"" << nametail <<"\"" << endl 
	   << "         Node_Color_ \"" << nodeColor<<"\"" << endl
	   << "         Node_Shape_ \"1\"" << endl
	   << "         Width_ \""<<size_s<<"\"" << endl
	   << "         Height_ \""<<size_s<<"\"" << endl
	   << "         Depth_ \""<<size_s<<"\"" << endl
	   << "         type Type_ \"[ 67108864 FILE_NODE ]\"" << endl;
    myfile << "         graphics [ type \"rectangle\" fill \"#FF0000\" ]" << endl
	   << "  ] " << endl;

    //    myfile << id << " [label=\"" << nametail <<"\",shape=none,height=.2,regular=0,color=\"Green\",fillcolor=green,fontname=\"7x13bold\",fontcolor=black,style=filled];\n";
    return id;
  } else {
    return i->second;
  }
}

int main(int ac, char* av[])
{
  
  if (ac < 2) {
    cerr << "Usage: " << av[0] << " databaseName" << endl;
    exit (1);
  }
  string filen = av[1];
  std::string filename = boost::filesystem::path(filen).leaf()+".gml";

  sqlite3_connection con;
  con.open(filen.c_str());

  std::ofstream myfile;
  myfile.open(filename.c_str());
  myfile << "graph CloneDetect [\ndirected\n" <<endl;
  // We need to do a pre-pass over all of the clusters before writing any out to remove clusters on the same files.
  map<vector<string>, vector<size_t> > clusters; // Map from file names in cluster (a vector with unique elements sorted by filename) to list of cluster numbers with those files
  try{
    std::string selectClusters ="SELECT cluster, file from postprocessed_clusters_joined order by cluster";
    cerr << selectClusters << endl;
    sqlite3_command cmd(con, selectClusters.c_str());
    sqlite3_reader datasets=cmd.executereader();
    string lastCluster = "";
    bool first = true;
    set<string> filesInThisCluster;
    // Read records (cluster elements) one at a time, gathering up the file
    // names in a given cluster.  When the end of a cluster is reached (i.e.,
    // either the end of the list of tuples or a new cluster number), add that
    // cluster into the clusters map.
    while (true) {
      // --- Check for and read current record
      bool haveRecord = datasets.read();
      string cluster;
      string filename;
      string path;
      if (haveRecord) {
        cluster = datasets.getstring(0);
        filename = datasets.getstring(1);
	//       path = boost::filesystem::path(filename).remove_leaf().string();
	//filename = boost::filesystem::path(filename).leaf();
	//        cerr << "Have record " << cluster << ": " << filename << endl;
      } else {
        // cerr << "End of data" << endl;
      }
      // --- Insert last cluster into map if needed
      bool needToGatherUpThisCluster = !first && (!haveRecord || cluster != lastCluster);
      if (needToGatherUpThisCluster) { // The current record, if any, is not part of the cluster being gathered up
        vector<string> filenameSortedVector(filesInThisCluster.begin(), filesInThisCluster.end());
        clusters[filenameSortedVector].push_back(boost::lexical_cast<size_t>(lastCluster));
      }
      // --- Add information for current record into current cluster
      if (!haveRecord) break; // The rest of this is to process the current record
      bool startingNewCluster = first || (cluster != lastCluster);
      first = false;
      if (startingNewCluster) { // Clear out any info from the last cluster
        filesInThisCluster.clear();
        lastCluster = cluster;
      }
      filesInThisCluster.insert(filename);
    }
  }catch(exception &ex) {
    cerr << "Exception occurred: " << ex.what() << endl;
  }

  map<string, string > filesize;
  try{
    std::string selectClusters ="SELECT file, sum(num_instructions) from function_statistics group by file ";
    cerr << selectClusters << endl;
    sqlite3_command cmd(con, selectClusters.c_str());
    sqlite3_reader datasets=cmd.executereader();
    while (datasets.read()) {
        string filename = datasets.getstring(0);
        string size_s = datasets.getstring(1);
	filesize[filename]=size_s;
    }
  }catch(exception &ex) {
    cerr << "Exception occurred: " << ex.what() << endl;
  }

  cerr << "clusterSize: " << clusters.size() << endl;
  size_t counter = clusters.size();
  double min = 1000;
  double max = 0;
  for (map<vector<string>, vector<size_t> >::const_iterator i = clusters.begin(); i != clusters.end(); ++i) {
    const vector<string>& filenames = i->first;
    const vector<size_t>& clusterNumbers = i->second;
    double weight = clusterNumbers.size();
    double len = 0.15+log(1+weight)*0.05;
    if (len<0.15) len = 0.15;
    if (len>max) max=len;
    if (len<min) min=len;
    //    if (len>0.7) len = 0.7;
    assert (filenames.size() >= 1); // A cluster should never be empty, or it would not have ended up in the table
    //if (clusterNumbers.size() < 5) continue;
    if (filenames.size() == 1) continue; // Skip clusters within a single file
    
    vector<size_t> fileIDs(filenames.size());
    for (size_t j = 0; j < filenames.size(); ++j) {
      fileIDs[j] = createOrGetFileID(myfile, filenames[j], filesize[filenames[j]]);
    }


    string edgeColor = clusterNumbers.size() >= 35 ? "FF0000" : clusterNumbers.size() >= 15 ? "FFCC66" : "FFFFCC";
    if (fileIDs.size() == 2) { // A cluster containing only two files, so create an undirected edge
      //      myfile << fileIDs[0] << " -- " << fileIDs[1] << " [color=\"" << edgeColor << "\", weight=" << weight << ", len=" << len << ", style=invis];\n";
      myfile << "  edge [ "
	     << "         label \"" << fileIDs[0] << "->" << fileIDs[1] <<"\"" << endl 
	     << "         source " << fileIDs[0] << " target " << fileIDs[1] << endl
	//     << "         Multiplicity_ \""<<count<<"\"" << endl
	//     << "         Edge_Radius_ \""<<count<<"\"" << endl
	     << "         Edge_Color_ \"" << edgeColor << "\"" << endl
	     << "  ] " << endl;
    } else { // A larger cluster, so there needs to be a separate node to represent the hyperedge
      ++counter;
      myfile << "  node [ id " << counter << endl 
	     << "         label \"" << counter <<"\"" << endl 
	     << "         Node_Color_ \"00FF00\"" << endl
	     << "         Node_Shape_ \"0\"" << endl
	     << "         Width_ \""<< len << "\"" << endl
	     << "         Height_ \""<< len << "\"" << endl
	     << "         Depth_ \""<< len << "\"" << endl
	     << "         Type_ \"[ 67108864 FUNCTION_NODE ]\"" << endl;
      myfile << "         graphics [ type \"ellipse\" fill \"#00FF00\" ]" << endl
	     << "  ] " << endl;
      //      myfile << counter << " [label=\"\", shape=point, width=.1, height=.1, color=\"" << edgeColor << "\", style=invis];\n";

      for (size_t j = 0; j < fileIDs.size(); ++j) {
	//        myfile << counter << " -- " << fileIDs[j] << "[ color=\"" << edgeColor << "\", weight=" << weight << ", len=" << (len / 2) << ", style=invis];\n";
	myfile << "  edge [ "
	       << "         label \"" << counter << "->" << fileIDs[j] <<"\"" << endl 
	       << "         source " << counter << " target " << fileIDs[j] << endl
	  //     << "         Multiplicity_ \""<<count<<"\"" << endl
	  //     << "         Edge_Radius_ \""<<count<<"\"" << endl
	       << "         Edge_Color_ \"" << edgeColor << "\"" << endl
	       << "  ] " << endl;
      }

    }
  }
  cerr << "Max: " <<max<<" min:" <<min<<endl;
  cerr << "Program done. "  << endl;
    
  myfile << "\n]\n" << endl;
  myfile.close();

  return 0;
};



