#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

class GraphStats {
  private:
    int n_graphs;
    int n_stacknodes;
    int n_heapnodes;
    vector<int> may_sets;
    vector<int> must_sets;
  
  public:
    GraphStats() { reset() ; }
   ~GraphStats() {}

  void addGraphs(int n) { n_graphs+=n; }
  void addStackNode()   { n_stacknodes++; }
  void addHeapNode()    { n_heapnodes++; }
  void addMayAliasCount(int n) { may_sets.push_back(n); }
  void addMustAliasCount(int n) { must_sets.push_back(n); }
  void reset() {
    n_graphs = 0;
	n_stacknodes  = 0;
	n_heapnodes  = 0;
	may_sets.clear();
	must_sets.clear();
  }
  
  string to_s() {
    int n = 0, may_sum = 0, must_sum = 0;
    for(vector<int>::const_iterator it=may_sets.begin(); it!=may_sets.end(); it++) {
	  n++;
	  may_sum += *it;
    }
    for(vector<int>::const_iterator it=must_sets.begin(); it!=must_sets.end(); it++) {
	  must_sum += *it;
    }
	stringstream str;
	str <<
	  "n: " << n  << "\n" <<
      "n_graphs: " << n_graphs << "\n" <<
      "n_stacknodes: " << n_stacknodes << "\n" <<
      "n_heapnodes: " << n_heapnodes << "\n" <<
      "n_nodes: " << n_stacknodes + n_heapnodes << "\n" <<
      "n_may-aliases: " << may_sum << "\n" <<
	  "avg_may-aliases: " << ((n==0)? 0 : (double)may_sum / (double)n) << "\n"
      "n_must-aliases: " << must_sum << "\n" <<
	  "avg_must-aliases: " << ((n==0)? 0 : (double)must_sum / (double)n) << "\n";
	return str.str();
  }

  void writeFile(char *fn) {
    ofstream out;
	out.open(fn);
	if (out) {
	  out << to_s();
	  out.close();
	}
  }
};

