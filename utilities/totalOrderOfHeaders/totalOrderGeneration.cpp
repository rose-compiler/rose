/*
Given an input path containing *.json files

input .json file has a format of 
 [["fullpath1", "name1", "0"],["fullpath2", "name2","1"],["fullpath3", "name3","1"], ...] 

The main() will Extract header full paths to creaet a partial order for each file, 
Then build a total order of the full paths.
And finally write #include <headerName> or #include "headerName", depending on the last flag 1 or 0.

g++ -std=c++11 totalOrderGeneration.cpp

./a.out .  # assuming .json files are in current path

There will be quite some debugging output. You can ignore them.

In the end, the headers will be written into a file named:
   total_header.hh

C. Liao, 12/27/2023

*/
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <algorithm> // For std::find
#include <fstream>
#include <sstream>
#include <cassert>
#include <dirent.h>  // For directory operations
#include <nlohmann/json.hpp>

using namespace std;

string hfile_name="total_header.hh";
// By default, this program will detect and remove cycles in the header dependency graph
// To facilitate debugging, users can disable the cycle removal step and the program will abort on the first cycle encountered.
bool abortOnCycle= false; 

// In the generated total header file, prepend #include "vxWorks.h"
bool includeVxWorksHeader = false; 

// prototypes of functions
void removeCycles(std::unordered_map<std::string, std::unordered_set<std::string>>& graph) ;

// a dict storing the input json's all fields: 
// header full path as the key, followed by its header name to be used in #include <header_name>, 
//              then if system "1" or user header "0"
unordered_map<string, vector<string>> headerInfo;

enum NodeState {
    UNVISITED,  // Node has not been visited yet
    VISITING,   // Node is currently being visited (on the active path being explored by the current call stack)
    VISITED     // Node has been visited and is no longer on the active path
};

// Helper function to perform Post-order DFS and topological sort
// 
// The key of the algorithm is the post-order traversal of the graph.
// We only add a node to the total order after we have visited all of its dependencies
// (all nodes it points to). 
// This ensures that all prerequisites of an elements are processed before the element itself.

// Adjacent lists to represent the dependency graph
//     each node is a header file
//     each edge represents a partial order between two header files.
// sortedStack stores a viable result
// We use vectors to mimic stacks. so we can iterate them from beginning also.

// Return false if a cycle is detected.

bool topologicalSortDFS(const string& node, // starting node
                        unordered_map<string, unordered_set<string>>& graph,
                        unordered_map<string, NodeState>& nodeStates,
                        vector<string>& sortedStack, // the final sorted nodes
                        vector<string>& pathStack) 
{
  // stop conditions:
  if (nodeStates[node] == VISITED) {
    // Node already processed
    return true;
  }
// TODO: real codes may have headers included in any order, introducing cycles
// We should just discard the edge bringing in cycle, treat it as if it were not there at all.
// This node is in the active path of the current call stack of the recursion.
// We are visiting it again. We just ignore this node?
// What is the new status transition scheme now? 
//
// A simplest solution: remove all cycles as a preprocessing step by removing the edges introducing cycles
  if (nodeStates[node] == VISITING) {
    // Cycle detected, print the cycle: using the node visited twice as beginning and end nodes.
    auto cycleStart = std::find(pathStack.begin(), pathStack.end(), node);
    cout << "Cycle detected: ";
    for (auto it = cycleStart; it != pathStack.end(); ++it) {
      cout << *it << " -> ";
    }
    cout << node << endl; // To complete the cycle
    if (abortOnCycle)
    {
       cerr << "Program aborted since cycle removal is disabled and abortOnCycle is set to true."<<endl;
       assert (false);
    }
    return false; // Cycle detected
  }

  // Set state to VISITING and add to pathStack
  // Do this before entering recursion to avoid infinite recursion call.
  nodeStates[node] = VISITING;
  pathStack.push_back(node);

  // DFS
  for (const auto& neighbor : graph[node]) {
    if (!topologicalSortDFS(neighbor, graph, nodeStates, sortedStack, pathStack)) {
      return false; // Cycle detected in a deeper call
    }
  }

  // Set state to VISITED, pop from pathStack, and add to sortedStack
  nodeStates[node] = VISITED;
  pathStack.pop_back(); // restore the active path

  sortedStack.push_back(node); // post-order processing of the current node
  return true;
}

// We generate a linear order from each translation unit
// We pass all linear order information as the input
// Return a viable total order. 
vector<string> generateTotalOrder(vector<vector<string>>& linearOrders, bool deleteCycles=false) 
{
  unordered_map<string, unordered_set<string>> graph;
  unordered_map<string, NodeState> state;

  vector<string> sortedStack;
  vector<string> pathStack;

  vector<string> totalOrder;

  // Construct the dependency graph
  // each neighboring pair is a partial order, an edge in the graph
  // for each linear order vector
  for (const auto& order : linearOrders) {
    // starting from first to the to last: we need to set flags for all.
    for (size_t i = 0; i < order.size(); ++i) {
      if (i+1 <order.size())
      { 
	// each node preceeds its next node: generating an edge in the dependency graph.
	graph[order[i]].insert(order[i + 1]);
      }

      // Mark each node as unvisited
      state[order[i]] = UNVISITED; 
    }
  } // end for all linear orders


// cycle detection and removal here
  if (deleteCycles)
    removeCycles (graph);


  // Perform topological sort
  for (const auto& s : state) {
    // if not yet visited
    if (s.second == UNVISITED) {
      if (!topologicalSortDFS(s.first, graph, state, sortedStack, pathStack)) 
      {
	return {}; // Cycle detected, return empty vector or handle appropriately
      }
    }
  }
  
  // TODO: we can use std::deque instead to enable efficient insertion of elements to both front and end.

  // the sorted stack will be in reversed order: we need to reverse it back. 
  while (!sortedStack.empty()) {
    totalOrder.push_back(sortedStack.back());
    sortedStack.pop_back();
  }

  return totalOrder;
}

// Helper function to print the total order from beginning to the end.
void printTotalOrder(const vector<string>& order) 
{
  for (const string& elem : order) {
    cout << elem << endl;
  }
}

// Test wrapper: input and expected output:
// Use non-const,  pass-by-value parameters: allowing in-place temp variables
void runTest(vector<vector<string>> linearOrders, 
             vector<string> expectedOrder, 
	     int testNumber, bool deleteCycles=false) 
{
  cout << "Running Test Case " << testNumber << ": ";

  auto totalOrder = generateTotalOrder(linearOrders, deleteCycles);

  if (totalOrder == expectedOrder) {
    cout << "PASS" << endl;
  } else {
    cout << "FAIL" << endl;
    cout << "Expected Order: ";
    for (const auto& item : expectedOrder) {
      cout << item << " ";
    }
    cout << "\nGenerated Order: ";
    for (const auto& item : totalOrder) {
      cout << item << " ";
    }
    cout << endl;
  }
}


// input .json file has a format of array of arrays of 3 strings
// [["path1", "name1", "0"],["path2", "name2","1"],["path3", "name3", "0"], ...] 
// This function:read the file by its name and 
//               extract three fields of each (path, name, flag) tuple and 
// Store the header info. into a map of string vs.  pair<>
// Also store and return all header paths into a vector of strings. 
vector<string> extractHeaderPaths(const std::string& filename) {
  vector<string> allPaths;
  // Open the file
  std::ifstream file(filename);
  if (!file.is_open()) {
    throw std::runtime_error("Unable to open file");
  }

  // Parse the JSON from the file
  nlohmann::json json;
  file >> json;

  // Ensure that the parsed object is an array
  if (!json.is_array()) {
    throw std::runtime_error("JSON is not an array");
  }

  // Iterate over the array
  for (const auto& item : json) {
    // Each item is in turn an array of 3 elements
    if (!item.is_array() || item.size() != 3) {
      throw std::runtime_error("Invalid array format");
    }

    // Extract path, name, and flag
    std::string path = item[0];
    std::string name = item[1];
    std::string flag = item[2];

    // Process or store these values as needed
    // For example, concatenate them and add to the paths vector
    headerInfo[path]={name, flag};
    allPaths.push_back(path);
  }

  return allPaths;
}
/*
   old manual parsing algorithm
   two loops:  over "]," or "]]"
   inner loop:  over ""," or """

   or a single loop , extract all fields, but group every 3 or them into a single record
   all possible leading prefix [["
*/

std::vector<std::string> extractHeaderPathsOld(const std::string& filename) {
  std::vector<std::string> paths;
  std::ifstream file(filename);
  std::string line;

  if (!file.is_open()) {
    std::cerr << "Unable to open input file: " << filename << std::endl;
    return paths;
  }

  cout<<"parsing a file:\t"<< filename<<endl;
  int iter=0;
  //while (getline(file, line, ',')) 
  while (getline(file, line, ']'))  //split before ]
  {
    std::stringstream ss(line);
    // jump out if nothing between last  and current ], the trailing two "]]"
    if  (line.size()==0) break;
    // another case : input file does not have headers included at all, empty []
    if  (line.size()==1) break;

    char bracket, quote, comma, last;
    // head path, header name, system or user flag
    std::string path, name, flag;

    if (iter==0) // first record starts with [[, we skip the first [
    {
      ss>>bracket; 
      assert (bracket == '[');
    }
    else  // later records start with ,[
    {
      ss>>comma; 
      assert (comma== ',');
    }

    // Extract the string up to the first comma, ignoring brackets and quotes
    //
    // check the first leading two chars: [" 
    ss >> bracket >> quote; // [" 
    if (bracket == '[' && quote == '"') { // matching [" 
      if (std::getline(ss, path, '"')) {  // Read up to the next quote, extracting the first element.
	paths.push_back(path);

        // check the second leading two chars: ," 
        // skip first ,"
	ss >> comma;
	ss >> quote;
        if (comma== ',' && quote == '"') {
          if (std::getline(ss, name, '"')) { // Read up to the next quote, extracting the name.

            // check the third leading two chars: ," 
            // skip second ,"
            ss >> comma;
            ss >> quote;

            if (comma== ',' && quote == '"') {
              if (std::getline(ss, flag, '"')) { // Read up to the next quote, extracting the name.
                headerInfo[path]={name, flag};
#if 0                
                cout<<"parsed a record:"<<endl;
                cout<<"\t"<< path<<endl;
                cout<<"\t"<<name<<endl;
                cout<<"\t"<<flag<<endl;
                // we reaches the end of "a", "b", "c"
#endif                // 
              }
              else
              {
                cerr<<"failed to extract the third field: flag before ," <<endl;
                assert (false);
              }
            }
            else
            {
              cerr<<"unexpected second comma and quote encoutered:" << comma<< " "  << quote<<endl;
              assert (false);
            }
          }
          else
          {
            cerr<<"failed to extract the second field: header name before ," <<endl;
            assert (false);
          }
        }
        else
        {
          cerr<<"unexpected leading comma and quote before the name field encoutered:" << comma<< " "  << quote<<endl;
          assert (false);
        }

      }
      else
      {
        cerr<<"failed to extract the first field for full header path before ," <<endl;
        assert (false);
      }
    }
    else
    {
      cerr<<"unexpected bracket and quote encoutered:" << bracket << " "  << quote<<endl;
      assert (false);
    }
    iter++; 
  }

  file.close();
  return paths;
}

// Builtin regression tests
int test() 
{
  // Test Case 1: Simple linear orders
  vector<vector<string>> linearOrders1 = {{"A", "B", "C"}, {"D", "E"}, {"E", "C"}};
  auto totalOrder1 = generateTotalOrder(linearOrders1);
  cout << "Total Order 1: "<<endl;

  printTotalOrder(totalOrder1);

  // Test Case 2: Single element in some linear orders
  vector<vector<string>> linearOrders2 = {{"A", "B"}, {"B", "C"}, {"D"}};
  auto totalOrder2 = generateTotalOrder(linearOrders2);
  cout << "Total Order 2: "<<endl;
  printTotalOrder(totalOrder2);

  // Test Case 3: Cycle detection
  vector<vector<string>> linearOrders3 = {{"A", "B"}, {"B", "C"}, {"C", "A"}};
  auto totalOrder3 = generateTotalOrder(linearOrders3);
  cout << "Total Order 3: "<<endl;
  if (totalOrder3.empty()) {
    cout << "Cycle detected. No total order possible." << endl;
  } else {
    printTotalOrder(totalOrder3);
  }

  // test 4: cycle removal
  auto totalOrder4 = generateTotalOrder(linearOrders3, true);
  cout << "Total Order 4: "<<endl;
  if (totalOrder4.empty()) {
    cout << "Cycle detected. No total order possible." << endl;
  } else {
    printTotalOrder(totalOrder4);
  }

  vector<vector<string>> linearOrders5 = {};
  auto totalOrder5 = generateTotalOrder(linearOrders5);
  cout << "Test Case 5 (No Linear Orders): ";
  printTotalOrder(totalOrder5);

  // more tests using the test wrapper
  // empty input
  runTest({}, {}, 0);
  runTest({{"A", "B", "C"}, {"D", "E"}}, {"A", "B", "C", "D", "E"}, 1);
  runTest({{"A", "B"}, {"B", "C"}, {"A", "D"}}, {"A", "B", "C", "D"}, 2);
  // single elements
  runTest({{"A"}, {"B"}, {"C"}}, {"A", "B", "C"}, 3);
 // overlapping
  runTest({{"A", "B"}, {"B", "C"}, {"C", "D"}, {"A", "D"}}, {"A", "B", "C", "D"}, 4);
  runTest({{"A", "A"}}, {}, 5); // self-loop

  runTest({{"A", "A"}}, {"A"}, 7, true); // self-loop, but with cycle deletions

  runTest({{"A", "B", "C"}, {"D", "E", "F"}, {"G", "H", "I"}}, {"D", "A", "B", "C", "E", "G", "F", "H", "I"}, 6);

  return 0;
}

// withe the sorted total order of header full paths
// write #include <headerName> or #include "headerName" based on its name and flag field stored in headerInfo
//
void writeHeaderfile(vector<string>& total_order, unordered_map<string, vector<string>>& headerInfo, string& filename)
{

  // Open the file in write mode
  std::ofstream hfile(filename);

  if (!hfile.is_open()) {
    std::cerr << "Failed to open file: " << filename << std::endl;
    assert(false);
  }

  if (includeVxWorksHeader)
    hfile<<"#include \"vxWorks.h\""<<endl;

  for (auto& header_path : total_order)
  {
    assert(headerInfo.count(header_path)==1);
    string name = headerInfo[header_path][0];
    string flag= headerInfo[header_path][1];

    string line ="#include ";
    if (flag=="0")
      line.push_back('"');
    else
    {
      assert (flag=="1");
      line.push_back('<');
    }

    line+=name; 

    if (flag=="0")
      line.push_back('"');
    else
    {
      assert (flag=="1");
      line.push_back('>');
    }

    hfile<<line<<endl;
  }

  hfile.close();
}

//--------------------------------------------------
// Within the context of header file order: a cycle means that the order of the involved header files does not matter.
// We can safely remove any edges introducing cycles. 

// Cycle detection and removal:
// Pre-order DFSa
// Return true if a cycle is detected. 
bool cycle_detection_dfs(const std::string& node, 
         std::unordered_map<std::string, bool>& visited, 
         std::unordered_map<std::string, bool>& recStack, // record nodes within the current active call stack path
         std::unordered_map<std::string, std::unordered_set<std::string>>& graph,
	 std::vector<std::pair<std::string, std::string>>& edgesToRemove) {

  // Mark the current node as visited and part of the recursion stack
  visited[node] = true;
  recStack[node] = true;

  for (const auto& adjNode : graph[node]) {
    if (!visited[adjNode]) {
      if (cycle_detection_dfs(adjNode, visited, recStack, graph, edgesToRemove))
	return true;
    } else if (recStack[adjNode]) { // visited and in active call stack path
      // Cycle detected, add the edge to the removal list
      edgesToRemove.push_back({node, adjNode});
      // immediate return once a cycle is detected.
      //minimize the modifications to the graph at each step.
      return true; // Remove this line if you want to find all cycles in one DFS call
    }
  }

  recStack[node] = false; // Remove the node from the recursion stack
  return false;
}

// find edges introducing 
void removeCycles(std::unordered_map<std::string, std::unordered_set<std::string>>& graph) {
  std::unordered_map<std::string, bool> visited;
  std::unordered_map<std::string, bool> recStack;
  std::vector<std::pair<std::string, std::string>> edgesToRemove;

  // starting for each node, doing the cycle detection
  // if there are remaining cycles: some nodes in the cycles will be used as the start nodes later to detect the cycle.
  for (const auto& pair : graph) {

    if (!visited[pair.first]) 
       cycle_detection_dfs(pair.first, visited, recStack, graph, edgesToRemove);
  }
  // Remove detected edges
  for (const auto& edge : edgesToRemove) {
    graph[edge.first].erase(edge.second);
  }

}


void show_help (const string& programName)
{
  std::cout << "Usage: " << programName << " [options]\n";
  std::cout << "Options:\n";
  std::cout << "  --help\tShow this help message\n";
  std::cout << "  --test\tRun the builtin test mode\n";
  std::cout << "  --abortOnCycle\t Disable cycle removal, abort and issue error messages when encountering first cyclic order in header files\n";
  std::cout << "  --verbose\tRun the program in a verbose mode\n";
  std::cout << "  --input=path\tSpecify the input path to search for input .json files recursively\n";
  std::cout << "  --output=filename\tSpecify the output header file's name, default is total_header.hh if not specified";
  std::cout << "  --include-vxworks-header\tIn the generated header file, prepend #include \"vxWorks.h\"";
}

//--------------------------------------------------
int main(int argc, char* argv[])
{
  bool inputProvided = false;
  bool verboseMode = false; 

  string directoryPath ="";

  std::string programName = argv[0];
  // Find the last occurrence of a path separator
  size_t lastSlash = programName.find_last_of("/\\");
  if (lastSlash != std::string::npos) {
    programName = programName.substr(lastSlash + 1);
  }

  if (argc < 2) {
    show_help(programName);
    return 1;
  }

  // Loop through command line arguments
  for (int i = 1; i < argc; ++i) 
  {
    std::string arg = argv[i];

    // Check for --help option
    if (arg == "--help") {
      show_help (programName);
      return 0;
    }

    // Check for --test option
    if (arg == "--test") {
      std::cout << "Test mode activated.\n";
      // Add test-related functionality here
      test();
      return 0;
    }

    if (arg== "--verbose")
    {
      verboseMode = true;
    }
    if (arg== "--include-vxworks-header")
    {
      includeVxWorksHeader = true;
    }
    // Check for --input option
    // note that .find() is used!
    else if (arg.find("--input=") == 0) 
    {
      inputProvided = true;
      directoryPath = arg.substr(8); // Remove "--input=" from the argument
//      std::cout << "Input path set to: " << inputPath << std::endl;
      // Add functionality to handle the input path
      // ...
    }
    else if (arg.find("--output=") == 0) 
    {
      hfile_name= arg.substr(9); // Remove "--output=" from the argument
//      std::cout << "output path set to: " << hfile_name<< std::endl;
    }
    else if (arg == "--abortOnCycle")
    {
      abortOnCycle = true;
    }
    else
    {
      cerr<<"Unrecognized argument:"<< arg<<endl;
      show_help(programName);
      return 0; 
    }
  }

  if (!inputProvided || directoryPath.size()==0) {
    std::cerr << "Error: No input path provided. Use '--input=path' to specify an input path.\n";
    return 1;
  }

  vector<vector<std::string>> allPaths;
  DIR *dir;
  struct dirent *ent;

  if ((dir = opendir(directoryPath.c_str())) != nullptr) {
    while ((ent = readdir(dir)) != nullptr) {
      std::string fileName = ent->d_name;
      if (fileName.length() > 5 && fileName.substr(fileName.length() - 5) == ".json") {
	std::vector<std::string> paths = extractHeaderPaths(directoryPath + "/" + fileName);
	allPaths.push_back(paths);
      }
    }
    closedir(dir);
  } else {
    std::cerr << "Could not open directory: " << directoryPath << std::endl;
    return 1;
  }

  if (verboseMode)
  {
    std::cout << "Extracted partial order is:" << std::endl; // Separator between files
    // Optional: Print all extracted paths for each file
    for (const auto& filePaths : allPaths) {
      std::cout << "-----" << std::endl; // Separator between files
      for (const std::string& path : filePaths) {
        std::cout << path << std::endl;
      }
    }
  }

  // Feed into the order generation function
  cout<<"Generating a total order for headers extracted from:"<< allPaths.size() << " json files"<<endl;
  cout<<"Abort On Cycles is set to be:"<< abortOnCycle <<endl;
  // if users do not specify abortOnCycle, we want to remove the cycles
  // If abortOnCycle, then we keep the cycles so the code can abort and issue warnings.
  bool removeCycle = !abortOnCycle; 
  auto totalOrder = generateTotalOrder(allPaths, removeCycle); // now add cycle deletion

  if (verboseMode)
  {
    std::cout << "-----" << std::endl; // Separator 
    cout << "Total Order is: "<<endl;
    std::cout << "-----" << std::endl; // Separator
    printTotalOrder(totalOrder);
  }

   cout<<"Writing out the resulting total order headers of count "<< totalOrder.size() <<" into:"<< hfile_name<<endl;
   writeHeaderfile(totalOrder, headerInfo, hfile_name);

  return 0;
}
