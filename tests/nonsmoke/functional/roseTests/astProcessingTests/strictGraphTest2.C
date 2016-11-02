#include <iostream>
#include <fstream>
//#include <rose.h>
#include <string>
#ifndef _MSC_VER
#include <err.h>
#endif
#include <boost/graph/adjacency_list.hpp>
#include "graphProcessing.h"
#include <assert.h>

using namespace std;
using namespace boost;



struct Vertex {
    int num;
};

struct Edge {
	int to;
	int from;
};

typedef boost::adjacency_list<
	boost::vecS,
	boost::vecS,
	boost::bidirectionalS,
	Vertex,
	Edge
> MyGraph;


typedef MyGraph::vertex_descriptor VertexID;
typedef MyGraph::edge_descriptor EdgeID;

class visitorTraversal : public SgGraphTraversal<MyGraph>
   {
     public:
      std::vector<std::vector<VertexID> > pths;
	  void analyzePath(vector<VertexID>& pth);
   };


void visitorTraversal::analyzePath(vector<VertexID>& pth) {
        //cout << "pths: " << paths.size() + 1 << std::endl;
        //cout << "pth.size(): " << pth.size() << std::endl;
        //ROSE_ASSERT(find(pths.begin(), paths.end(), pth) != paths.end());
            std::cout << "path" << std::endl;
            for (unsigned int i = 0; i < pth.size(); i++) {
                std::cout << vertintmap[pth[i]] << ", ";
            }
            std::cout << "end" << std::endl;
        //}
        ROSE_ASSERT(find(pths.begin(), pths.end(), pth) == pths.end());
        if (find(pths.begin(), pths.end(), pth) == pths.end()) {
            pths.push_back(pth);
        }
        

}

int main() {

       MyGraph* graph1 = new MyGraph;
       MyGraph* graph2 = new MyGraph;
       MyGraph* graph3 = new MyGraph;
       MyGraph* graph4 = new MyGraph;
       //MyGraph* graph5 = new MyGraph;
 
       VertexID vidComplex[21];
       for (unsigned int j = 0; j < 4; j++) {
           vidComplex[j] = boost::add_vertex(*graph1);
       }
       for (unsigned int j = 4; j < 9; j++) {
           vidComplex[j] = boost::add_vertex(*graph2);
       }
       for (unsigned int j = 9; j < 14; j++) {
           vidComplex[j] = boost::add_vertex(*graph3);
       }
       for (unsigned int j = 14; j < 20; j++) {
           vidComplex[j] = boost::add_vertex(*graph4);
       }






       bool ok;

       EdgeID g1;
       boost::tie(g1, ok) = boost::add_edge(vidComplex[0], vidComplex[1], *graph1);
       EdgeID g2;
       boost::tie(g2, ok) = boost::add_edge(vidComplex[1], vidComplex[2], *graph1);
       EdgeID g3;
       boost::tie(g3, ok) = boost::add_edge(vidComplex[2], vidComplex[3], *graph1);
       EdgeID g4;
       boost::tie(g4, ok) = boost::add_edge(vidComplex[3], vidComplex[0], *graph1);
       

       EdgeID g5;
       boost::tie(g5, ok) = boost::add_edge(vidComplex[4], vidComplex[5], *graph2);
       EdgeID g6;
       boost::tie(g6, ok) = boost::add_edge(vidComplex[5], vidComplex[6], *graph2);
       EdgeID g7;
       boost::tie(g7, ok) = boost::add_edge(vidComplex[6], vidComplex[7], *graph2);
       EdgeID g8;
       boost::tie(g8, ok) = boost::add_edge(vidComplex[6], vidComplex[5], *graph2);
       EdgeID g9;
       boost::tie(g9, ok) = boost::add_edge(vidComplex[7], vidComplex[6], *graph2);
       EdgeID g10;
       boost::tie(g10, ok) = boost::add_edge(vidComplex[7], vidComplex[5], *graph2);
       EdgeID g11;
       boost::tie(g11, ok) = boost::add_edge(vidComplex[5], vidComplex[7], *graph2);
       EdgeID g12;
       boost::tie(g12, ok) = boost::add_edge(vidComplex[5], vidComplex[8], *graph2);



       EdgeID g13;
       boost::tie(g13, ok) = boost::add_edge(vidComplex[9], vidComplex[10], *graph3);
       EdgeID g14;
       boost::tie(g14, ok) = boost::add_edge(vidComplex[10], vidComplex[11], *graph3);
       EdgeID g15;
       boost::tie(g15, ok) = boost::add_edge(vidComplex[11], vidComplex[12], *graph3);
       EdgeID g16;
       boost::tie(g16, ok) = boost::add_edge(vidComplex[12], vidComplex[13], *graph3);
       EdgeID g17;
       boost::tie(g17, ok) = boost::add_edge(vidComplex[12], vidComplex[10], *graph3);
       EdgeID g18;
       boost::tie(g18, ok) = boost::add_edge(vidComplex[10], vidComplex[13], *graph3);


       EdgeID g19;
       boost::tie(g19, ok) = boost::add_edge(vidComplex[14], vidComplex[15], *graph4);
       EdgeID g20;
       boost::tie(g20, ok) = boost::add_edge(vidComplex[15], vidComplex[16], *graph4);
       EdgeID g21;
       boost::tie(g21, ok) = boost::add_edge(vidComplex[16], vidComplex[17], *graph4);
       EdgeID g22;
       boost::tie(g22, ok) = boost::add_edge(vidComplex[17], vidComplex[18], *graph4);
       EdgeID g23;
       boost::tie(g23, ok) = boost::add_edge(vidComplex[18], vidComplex[15], *graph4);
       EdgeID g24;
       boost::tie(g24, ok) = boost::add_edge(vidComplex[15], vidComplex[18], *graph4);
       EdgeID g25;
       boost::tie(g25, ok) = boost::add_edge(vidComplex[18], vidComplex[16], *graph4);
       EdgeID g26;
       boost::tie(g26, ok) = boost::add_edge(vidComplex[16], vidComplex[18], *graph4);
       EdgeID g27;
       boost::tie(g27, ok) = boost::add_edge(vidComplex[18], vidComplex[19], *graph4);

 /*
       VertexID vID3[10];
       MyGraph* graph3 = new MyGraph;
       for (unsigned int j = 0; j < 10; j++) {
           vID3[j] = boost::add_vertex(*graph3);
       }
       bool ok;
       EdgeID g1;
       boost::tie(g1, ok) = boost::add_edge(vID3[0], vID3[1], *graph3);
       EdgeID g2;
       boost::tie(g2, ok) = boost::add_edge(vID3[1], vID3[2], *graph3);
       EdgeID g3;
       boost::tie(g3, ok) = boost::add_edge(vID3[2], vID3[4], *graph3);
       EdgeID g4;
       boost::tie(g4, ok) = boost::add_edge(vID3[4], vID3[3], *graph3);
       EdgeID g5;
       boost::tie(g5, ok) = boost::add_edge(vID3[3], vID3[5], *graph3);
       EdgeID g6;
       boost::tie(g6, ok) = boost::add_edge(vID3[5], vID3[6], *graph3);
       EdgeID g7;
       boost::tie(g7, ok) = boost::add_edge(vID3[6], vID3[3], *graph3);
       EdgeID g8;
       boost::tie(g8, ok) = boost::add_edge(vID3[6], vID3[4], *graph3);
       EdgeID g9;
       boost::tie(g9, ok) = boost::add_edge(vID3[4], vID3[7], *graph3);
       EdgeID g10;
       boost::tie(g10, ok) = boost::add_edge(vID3[3], vID3[1], *graph3);
       EdgeID g11;
       boost::tie(g11, ok) = boost::add_edge(vID3[5], vID3[7], *graph3);
       visitorTraversal* xis2 = new visitorTraversal();
       std::vector<std::vector<VertexID> > pth;
       //xis2->pths = pth;
  std::vector<std::vector<int> > mypath;
 int myints[] = {1, 2, 3, 5, 4, 2, 3, 5, 4, 6, 7, 5, 8};
  vector<int> fifth (myints, myints + sizeof(myints) / sizeof(int) );
mypath.push_back(fifth);
int myints1[] = {1, 2, 3, 5, 4, 6, 7, 4, 6, 7, 5, 8};
vector<int> fifth1 (myints1, myints1 + sizeof(myints1) / sizeof(int) );
mypath.push_back(fifth1);
//int myints2[] = {1, 2, 3, 5, 4, 6, 7, 5, 4, 6, 7, 5, 8};
//vector<int> fifth2 (myints2, myints2 + sizeof(myints2) / sizeof(int) );
//mypath.push_back(fifth2);
int myints3[] = {1, 2, 3, 5, 4, 6, 7, 5, 8};
vector<int> fifth3 (myints3, myints3 + sizeof(myints3) / sizeof(int) );
mypath.push_back(fifth3);
int myints4[] = {1, 2, 3, 5, 4, 2, 3, 5, 8};
vector<int> fifth4 (myints4, myints4 + sizeof(myints4) / sizeof(int) );
mypath.push_back(fifth4);
//int myints5[] = {1, 2, 3, 5, 4, 2, 3, 5, 4, 2, 3, 5, 8};
//vector<int> fifth5 (myints5, myints5 + sizeof(myints5) / sizeof(int) );
//mypath.push_back(fifth5);
int myints6[] = {1, 2, 3, 5, 4, 6, 7, 4, 2, 3, 5, 8};
std::vector<int> fifth6 (myints6, myints6 + sizeof(myints6) / sizeof(int) );
mypath.push_back(fifth6);
int myints7[] = {1, 2, 3, 5, 4, 6, 7, 5, 4, 2, 3, 5, 8};
std::vector<int> fifth7 (myints7, myints7 + sizeof(myints7) / sizeof(int) );
mypath.push_back(fifth7);
int myints8[] = {1, 2, 3, 5, 8};
std::vector<int> fifth8 (myints8, myints8 + sizeof(myints8) / sizeof(int) );
mypath.push_back(fifth8);
int myints9[] = {9};
std::vector<int> fifth9 (myints9, myints9 + sizeof(myints9) / sizeof(int) );
mypath.push_back(fifth9);
int myints10[] = {10};
std::vector<int> fifth10 (myints10, myints10 + sizeof(myints10) / sizeof(int) );
mypath.push_back(fifth10);
int myints11[] = {1, 2, 3, 5, 4, 2, 3, 5, 4, 6, 8};
std::vector<int> fifth11 (myints11, myints11 + sizeof(myints11) / sizeof(int) );
mypath.push_back(fifth11);

int myints12[] = {1, 2, 3, 5, 4, 6, 7, 4, 6, 8};
std::vector<int> fifth12 (myints12, myints12 + sizeof(myints12) / sizeof(int) );
mypath.push_back(fifth12);

int myints13[] = {1, 2, 3, 5, 4, 6, 7, 5, 4, 6, 8};
std::vector<int> fifth13 (myints13, myints13 + sizeof(myints13) / sizeof(int) );
mypath.push_back(fifth13);

int myints14[] = {1, 2, 3, 5, 4, 6, 8};
std::vector<int> fifth14 (myints14, myints14 + sizeof(myints14) / sizeof(int) );
mypath.push_back(fifth14);

int Emyints15[] = {1, 2, 3, 5, 4, 6, 7, 4, 2, 3, 5, 4, 6, 7, 5, 8};
std::vector<int> Efifth15 (Emyints15, Emyints15 + sizeof(Emyints15) / sizeof(int) );
mypath.push_back(Efifth15);

int Emyints16[] = {1, 2, 3, 5, 4, 6, 7, 4, 2, 3, 5, 4, 6, 8};
std::vector<int> Efifth16 (Emyints16, Emyints16 + sizeof(Emyints16) / sizeof(int) );
mypath.push_back(Efifth16);



*/
visitorTraversal* xis1 = new visitorTraversal;
       std::cout << "begin xis1: " << std::endl;
       xis1->constructPathAnalyzer(graph1, false, vidComplex[0], vidComplex[0]);
       std::cout << "end xis1" << std::endl;

visitorTraversal* xis2 = new visitorTraversal;
       std::cout << "begin xiz2" << std::endl;
       xis2->constructPathAnalyzer(graph2, false, vidComplex[4], vidComplex[8]);
       std::cout << "end xiz2" << std::endl;

visitorTraversal* xis3 = new visitorTraversal;

       std::cout << "begin xis3" << std::endl;
       xis3->constructPathAnalyzer(graph3, false, vidComplex[9], vidComplex[13]);
       std::cout << "end xis3" << std::endl;

visitorTraversal* xis4 = new visitorTraversal;
       std::cout << "begin xis4" << std::endl;
       xis4->constructPathAnalyzer(graph4, false, vidComplex[14], vidComplex[19]);
       std::cout << "end xis4" << std::endl;


std::vector<int> npth;

for (unsigned int i = 0; i < xis1->pths.size(); i++) {
    for (unsigned int j = 0; j < xis1->pths[i].size(); j++) {
        npth.push_back(xis1->vertintmap[xis1->pths[i][j]]);
       std::cout << npth.back() << ", ";
    }
    npth.clear();
    std::cout << std::endl;
}
std::cout << std::endl;
/*
    //std::cout << std::endl;
    if (find(mypath.begin(), mypath.end(), npth) == mypath.end()) {
        std::cout << "poss new path: " << std::endl;
        for (int qqw = 0; qqw < npth.size(); qqw++) {
            std::cout << npth[qqw] << ", ";
        }
        std::cout << std::endl;
        ROSE_ASSERT(false);
    }
    npth.clear();
}
*/
//std::cout << "xis3->pths.size()" << xis3->pths.size() << std::endl;
//std::cout << "mypath2.size()" << mypath2.size() << std::endl;
//ROSE_ASSERT(mypath2.size() == xis3->pths.size());
for (unsigned int i = 0; i < xis2->pths.size(); i++) {
    for (unsigned int j = 0; j < xis2->pths[i].size(); j++) {
        npth.push_back(xis2->vertintmap[xis2->pths[i][j]]);
        std::cout << npth.back() << ", ";
    }
    //std::cout << std::endl;
    //ROSE_ASSERT(find(mypath2.begin(), mypath2.end(), npth) != mypath2.end());
    npth.clear();
    std::cout << std::endl;
}
std::cout << std::endl;
//std::cout << "xis4->pths.size()" << xis4->pths.size() << std::endl;
//std::cout << "mypath3.size()" << mypath3.size() << std::endl;
//ROSE_ASSERT(mypath3.size() == xis4->pths.size());
for (unsigned int i = 0; i < xis3->pths.size(); i++) {
//    if (find(mypath3.begin(), mypath3.end(), xis4->pths[i]) == mypath3.end()) {
//        std::cout << "poss new path: " << std::endl;
//        for (int qqe = 0; qqe < xis4->pths[i].size(); qqe++) {
//            std::cout << xis4->pths[i][qqe] << ", ";
//        }
//        std::cout << "end" << std::endl;
//    }i
    for (unsigned int j = 0; j < xis3->pths[i].size(); j++) {
        npth.push_back(xis3->vertintmap[xis3->pths[i][j]]);
        std::cout << npth.back() << ", ";
    }
    std::cout << std::endl;
    //if (find(mypath3.begin(), mypath3.end(), npth) == mypath3.end()) {
    //    std::cout << "path not in mypath3: " << std::endl;
    //    for (int qu = 0; qu < npth.size(); qu++) {
    //        std::cout << npth[qu] << ", ";
    //    }
    //    std::cout << std::endl;
        //ROSE_ASSERT(false);
    
    npth.clear();
    }
    std::cout << std::endl;
//ROSE_ASSERT(mypath3.size() == xis4->pths.size());

for (unsigned int i = 0; i < xis4->pths.size(); i++) {
    for (unsigned int j = 0; j < xis4->pths[i].size(); j++) {
        npth.push_back(xis4->vertintmap[xis4->pths[i][j]]);
       std::cout << npth.back() << ", ";
    }
    npth.clear();
    std::cout << std::endl;
}
std::cout << std::endl;


	return 0;
	
}  
