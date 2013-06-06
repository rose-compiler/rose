
#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <iostream>

#include "sqlite3x.h"

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <fstream>

#include <string>
#include <cstdio> // for std::remove
#include <istream>
using namespace std;
using namespace sqlite3x;
using namespace boost::program_options;

// If the file has already been created as a node, get its ID.
// If not, create an ID for it and output its node declaration to myfile.
size_t createOrGetFileID(ostream& myfile, const string& filename) {
  static map<string, size_t> fileIDs;
  map<string, size_t>::const_iterator i = fileIDs.find(filename);
  if (i == fileIDs.end()) {
    size_t id = fileIDs.size();
    fileIDs.insert(std::make_pair(filename, id));
    string nametail = boost::filesystem::path(filename).leaf();
    if (nametail.rfind('-') != string::npos) { // Remove "expanded-" prefix
      nametail = nametail.substr(nametail.rfind('-') + 1);
    }
    myfile << id << " [label=\"" << nametail <<"\",shape=none,height=.2,regular=0,color=\"Green\",fillcolor=green,fontname=\"7x13bold\",fontcolor=black,style=filled];\n";
    // myfile << id << " [label=\"\",shape=point,height=.5,regular=0,color=\"Green\",fillcolor=green,fontname=\"7x13bold\",fontcolor=black,style=filled];\n";
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
  std::string filename = boost::filesystem::path(filen).leaf()+".dot";

  sqlite3_connection con;
  con.open(filen.c_str());

  std::ofstream myfile;
  myfile.open(filename.c_str());
  myfile << "graph CloneDetect {" <<endl;
  // We need to do a pre-pass over all of the clusters before writing any out to remove clusters on the same files.
  map<vector<string>, vector<size_t> > clusters; // Map from file names in cluster (a vector with unique elements sorted by filename) to list of cluster numbers with those files
  try{
    std::string selectClusters ="SELECT cluster, file from clusters order by cluster";
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
      if (haveRecord) {
        cluster = datasets.getstring(0);
        filename = datasets.getstring(1);
        filename = boost::filesystem::path(filename).leaf();
        // cerr << "Have record " << cluster << ": " << filename << endl;
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

  size_t counter = clusters.size();
  for (map<vector<string>, vector<size_t> >::const_iterator i = clusters.begin(); i != clusters.end(); ++i) {
    const vector<string>& filenames = i->first;
    const vector<size_t>& clusterNumbers = i->second;
    double weight = clusterNumbers.size();
    double len = 1. / weight;
    assert (filenames.size() >= 1); // A cluster should never be empty, or it would not have ended up in the table
    if (clusterNumbers.size() < 5) continue;
    if (filenames.size() == 1) continue; // Skip clusters within a single file
    vector<size_t> fileIDs(filenames.size());
    for (size_t j = 0; j < filenames.size(); ++j) {
      fileIDs[j] = createOrGetFileID(myfile, filenames[j]);
    }
    string edgeColor = clusterNumbers.size() >= 35 ? "darkred" : clusterNumbers.size() >= 15 ? "red" : "pink";
    if (fileIDs.size() == 1) { // A cluster between parts of the same file, so output it as a self-loop
      // myfile << fileIDs[0] << " -- " << fileIDs[0] << ";\n";
    } else if (fileIDs.size() == 2) { // A cluster containing only two files, so create an undirected edge
      myfile << fileIDs[0] << " -- " << fileIDs[1] << " [color=\"" << edgeColor << "\", weight=" << weight << ", len=" << len << ", style=invis];\n";
    } else { // A larger cluster, so there needs to be a separate node to represent the hyperedge
      ++counter;
      myfile << counter << " [label=\"\", shape=point, width=.1, height=.1, color=\"" << edgeColor << "\", style=invis];\n";
      // myfile << counter << " [label=\"\", shape=point, width=.01, height=.01];\n";
      for (size_t j = 0; j < fileIDs.size(); ++j) {
        myfile << counter << " -- " << fileIDs[j] << "[ color=\"" << edgeColor << "\", weight=" << weight << ", len=" << (len / 2) << ", style=invis];\n";
      }
    }
  }

  myfile << "}" << endl;
  myfile.close();

  return 0;
};



