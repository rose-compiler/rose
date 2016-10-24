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



std::vector<std::vector<int> > mypath2;
int myints15[] = {4, 6, 7, 4};
std::vector<int> fifth15 (myints15, myints15 + sizeof(myints15) / sizeof(int) );
mypath2.push_back(fifth15);
int myints16[] = {4, 6, 7, 5, 4};
std::vector<int> fifth16 (myints16, myints16 + sizeof(myints16) / sizeof(int) );
mypath2.push_back(fifth16);
//int myints17[] = {4, 6, 7, 5, 8};
//std::vector<int> fifth17 (myints17, myints17 + sizeof(myints17) / sizeof(int) );
//mypath2.push_back(fifth17);
//int myints18[] = {4, 6, 8};
//std::vector<int> fifth18 (myints18, myints18 + sizeof(myints18) / sizeof(int) );
//mypath2.push_back(fifth18);
int myints19[] = {4, 2, 3, 5, 4};
std::vector<int> fifth19 (myints19, myints19 + sizeof(myints19) / sizeof(int) );
mypath2.push_back(fifth19);
//int myints20[] = {4, 2, 3, 5, 8};
//std::vector<int> fifth20 (myints20, myints20 + sizeof(myints20) / sizeof(int) );
//mypath2.push_back(fifth20);

std::vector<std::vector<int> > mypath3;
int myints21[] = {7, 4, 6, 8};
std::vector<int> fifth21 (myints21, myints21 + sizeof(myints21) / sizeof(int) );
mypath3.push_back(fifth21);
int myints22[] = {7, 4, 2, 3, 5, 4, 2, 3, 5, 8};
std::vector<int> fifth22 (myints22, myints22 + sizeof(myints22) / sizeof(int) );
mypath3.push_back(fifth22);
int myints23[] = {7, 4, 6, 7, 4, 2, 3, 5, 8};
std::vector<int> fifth23 (myints23, myints23 + sizeof(myints23) / sizeof(int) );
mypath3.push_back(fifth23);
int myints24[] = {7, 4, 6, 7, 5, 4, 2, 3, 5, 8};
std::vector<int> fifth24 (myints24, myints24 + sizeof(myints24) / sizeof(int) );
mypath3.push_back(fifth24);
int myints25[] = {7, 4, 2, 3, 5, 8};
std::vector<int> fifth25 (myints25, myints25 + sizeof(myints25) / sizeof(int) );
mypath3.push_back(fifth25);
int myints26[] = {7, 5, 4, 2, 3, 5, 4, 6, 8};
std::vector<int> fifth26 (myints26, myints26 + sizeof(myints26) / sizeof(int) );
mypath3.push_back(fifth26);
int myints27[] = {7, 5, 4, 6, 7, 4, 6, 8};
std::vector<int> fifth27 (myints27, myints27 + sizeof(myints27) / sizeof(int) );
mypath3.push_back(fifth27);
int myints28[] = {7, 5, 4, 6, 7, 5, 4, 6, 8};
std::vector<int> fifth28 (myints28, myints28 + sizeof(myints28) / sizeof(int) );
mypath3.push_back(fifth28);
int myints29[] = {7, 5, 4, 6, 8};
std::vector<int> fifth29 (myints29, myints29 + sizeof(myints29) / sizeof(int) );
mypath3.push_back(fifth29);
int myints30[] = {7, 5, 4, 2, 3, 5, 8};
std::vector<int> fifth30 (myints30, myints30 + sizeof(myints30) / sizeof(int) );
mypath3.push_back(fifth30);
//int myints31[] = {7, 5, 4, 2, 3, 5, 4, 2, 3, 5, 8};
//std::vector<int> fifth31 (myints31, myints31 + sizeof(myints31) / sizeof(int) );
//mypath3.push_back(fifth31);
int myints32[] = {7, 5, 4, 2, 3, 5, 4, 6, 7, 5, 8};
std::vector<int> fifth32 (myints32, myints32 + sizeof(myints32) / sizeof(int) );
mypath3.push_back(fifth32);
int myints33[] = {7, 5, 4, 6, 7, 4, 2, 3, 5, 8};
std::vector<int> fifth33 (myints33, myints33 + sizeof(myints33) / sizeof(int) );
mypath3.push_back(fifth33);
int myints34[] = {7, 5, 4, 6, 7, 4, 6, 7, 5, 8};
std::vector<int> fifth34 (myints34, myints34 + sizeof(myints34) / sizeof(int) );
mypath3.push_back(fifth34);
int myints35[] = {7, 5, 4, 6, 7, 5, 8};
std::vector<int> fifth35 (myints35, myints35 + sizeof(myints35) / sizeof(int) );
mypath3.push_back(fifth35);
int myints36[] = {7, 5, 4, 6, 7, 5, 4, 2, 3, 5, 8};
std::vector<int> fifth36 (myints36, myints36 + sizeof(myints36) / sizeof(int) );
mypath3.push_back(fifth36);
//int myints37[] = {7, 5, 4, 6, 7, 5, 4, 6, 7, 5, 8};
//std::vector<int> fifth37 (myints37, myints37 + sizeof(myints37) / sizeof(int) );
//mypath3.push_back(fifth37);
int myints38[] = {7, 5, 8};
std::vector<int> fifth38 (myints38, myints38 + sizeof(myints38) / sizeof(int) );
mypath3.push_back(fifth38);
int myints39[] = {7, 4, 2, 3, 5, 4, 6, 7, 4, 6, 8};
std::vector<int> fifth39 (myints39, myints39 + sizeof(myints39) / sizeof(int) );
mypath3.push_back(fifth39);
int myints40[] = {7, 4, 6, 7, 4, 6, 8};
std::vector<int> fifth40 (myints40, myints40 + sizeof(myints40) / sizeof(int) );
mypath3.push_back(fifth40);
int myints41[] = {7, 5, 4, 2, 3, 5, 4, 6, 7, 4, 6, 8};
std::vector<int> fifth41 (myints41, myints41 + sizeof(myints41) / sizeof(int) );
mypath3.push_back(fifth41);

int myints42[] = {7, 4, 2, 3, 5, 4, 6, 8};
std::vector<int> fifth42 (myints42, myints42 + sizeof(myints42) / sizeof(int) );
mypath3.push_back(fifth42);

int myints43[] = {7, 4, 2, 3, 5, 4, 6, 7, 4, 2, 3, 5, 8};
std::vector<int> fifth43 (myints43, myints43 + sizeof(myints43) / sizeof(int) );
mypath3.push_back(fifth43);

int myints44[] = {7, 5, 4, 2, 3, 5, 4, 6, 7, 4, 2, 3, 5, 8};
std::vector<int> fifth44 (myints44, myints44 + sizeof(myints44) / sizeof(int) );
mypath3.push_back(fifth44);



visitorTraversal* xis3 = new visitorTraversal;
       std::cout << "begin xis3: " << std::endl;
       xis3->constructPathAnalyzer(graph3, false, vID3[3], vID3[3]);
       std::cout << "end xis3" << std::endl;
visitorTraversal* xis4 = new visitorTraversal;
       std::cout << "begin xiz4" << std::endl;
       xis4->constructPathAnalyzer(graph3, false, vID3[6], vID3[7]);
       std::cout << "end xiz4" << std::endl;
       std::cout << "begin xis2" << std::endl;
       xis2->constructPathAnalyzer(graph3, false, vID3[0], vID3[7]);
       std::cout << "end xis2" << std::endl;
       //std::cout << "pths" << (xis2->paths).size() << std::endl;
       std::vector<int> npth;
for (unsigned int i = 0; i < xis2->pths.size(); i++) {
    for (unsigned int j = 0; j < xis2->pths[i].size(); j++) {
        npth.push_back(xis2->vertintmap[xis2->pths[i][j]]);
       // std::cout << npth.back() << ", ";
    }
    //std::cout << std::endl;
    if (find(mypath.begin(), mypath.end(), npth) == mypath.end()) {
        std::cout << "poss new path: " << std::endl;
        for (unsigned int qqw = 0; qqw < npth.size(); qqw++) {
            std::cout << npth[qqw] << ", ";
        }
        std::cout << std::endl;
        //ROSE_ASSERT(false);
    }
    npth.clear();
}
std::cout << "xis3->pths.size()" << xis3->pths.size() << std::endl;
std::cout << "mypath2.size()" << mypath2.size() << std::endl;
ROSE_ASSERT(mypath2.size() == xis3->pths.size());
for (unsigned int i = 0; i < xis3->pths.size(); i++) {
    for (unsigned int j = 0; j < xis3->pths[i].size(); j++) {
        npth.push_back(xis3->vertintmap[xis3->pths[i][j]]);
       // std::cout << npth.back() << ", ";
    }
    //std::cout << std::endl;
    ROSE_ASSERT(find(mypath2.begin(), mypath2.end(), npth) != mypath2.end());
    npth.clear();
}
std::cout << "xis4->pths.size()" << xis4->pths.size() << std::endl;
std::cout << "mypath3.size()" << mypath3.size() << std::endl;
//ROSE_ASSERT(mypath3.size() == xis4->pths.size());
for (unsigned int i = 0; i < xis4->pths.size(); i++) {
//    if (find(mypath3.begin(), mypath3.end(), xis4->pths[i]) == mypath3.end()) {
//        std::cout << "poss new path: " << std::endl;
//        for (int qqe = 0; qqe < xis4->pths[i].size(); qqe++) {
//            std::cout << xis4->pths[i][qqe] << ", ";
//        }
//        std::cout << "end" << std::endl;
//    }i
    for (unsigned int j = 0; j < xis4->pths[i].size(); j++) {
        npth.push_back(xis4->vertintmap[xis4->pths[i][j]]);
       // std::cout << npth.back() << ", ";
    }
    std::cout << std::endl;
    if (find(mypath3.begin(), mypath3.end(), npth) == mypath3.end()) {
        std::cout << "path not in mypath3: " << std::endl;
        for (unsigned int qu = 0; qu < npth.size(); qu++) {
            std::cout << npth[qu] << ", ";
        }
        std::cout << std::endl;
        //ROSE_ASSERT(false);
    }
    npth.clear();
}
//ROSE_ASSERT(mypath3.size() == xis4->pths.size());


       std::cout << "next" << std::endl;
       VertexID vID2[10];
       MyGraph* graph2 = new MyGraph;
       for (unsigned int j = 0; j < 10; j++) {
           vID2[j] = boost::add_vertex(*graph2);
       }
       EdgeID f1;
       boost::tie(f1, ok) = boost::add_edge(vID2[0], vID2[1], *graph2);
       EdgeID f2;
       boost::tie(f2, ok) = boost::add_edge(vID2[1], vID2[2], *graph2);
       EdgeID f3;
       boost::tie(f3, ok) = boost::add_edge(vID2[2], vID2[3], *graph2);
       EdgeID f4;
       boost::tie(f4, ok) = boost::add_edge(vID2[3], vID2[1], *graph2);
       EdgeID f5;
       boost::tie(f5, ok) = boost::add_edge(vID2[3], vID2[4], *graph2);
       EdgeID f6;
       boost::tie(f6, ok) = boost::add_edge(vID2[2], vID2[4], *graph2);
       visitorTraversal* xis = new visitorTraversal();
       xis->pths = pth;
       xis->constructPathAnalyzer(graph2, false, vID2[0], vID2[4]);
      // std::cout << "pths" << (xis->paths).size() << std::endl;


	std::map<VertexID, int> numMap;
	VertexID vID[20];
	MyGraph* graph = new MyGraph;
	for (unsigned int i = 0; i < 20; i++) {
		vID[i] = boost::add_vertex(*graph);
		numMap[vID[i]] = i+1;
	}
	EdgeID e1;
	boost::tie(e1, ok) = boost::add_edge(vID[0], vID[1], *graph);
	if (!ok) {
		ROSE_ASSERT(false);
	}
    EdgeID e2;
    boost::tie(e2, ok) = boost::add_edge(vID[1], vID[2], *graph);
    EdgeID e3;
    boost::tie(e3, ok) = boost::add_edge(vID[1], vID[3], *graph);
    EdgeID e4;
    boost::tie(e4, ok) = boost::add_edge(vID[2], vID[7], *graph);
    EdgeID e5;
    boost::tie(e5, ok) = boost::add_edge(vID[3], vID[7], *graph);
    EdgeID e6;
    boost::tie(e6, ok) = boost::add_edge(vID[7], vID[8], *graph);
    EdgeID e7;
    boost::tie(e7, ok) = boost::add_edge(vID[6], vID[8], *graph);
    EdgeID e8;
    boost::tie(e8, ok) = boost::add_edge(vID[2], vID[9], *graph);
    EdgeID e9;
    boost::tie(e9, ok) = boost::add_edge(vID[8], vID[9], *graph);
    EdgeID e10;
    boost::tie(e10, ok) = boost::add_edge(vID[8], vID[10], *graph);
    EdgeID e11;
    boost::tie(e11, ok) = boost::add_edge(vID[9], vID[11], *graph);
    EdgeID e12;
    boost::tie(e12, ok) = boost::add_edge(vID[5], vID[11], *graph);
    EdgeID e13;
    boost::tie(e13, ok) = boost::add_edge(vID[10], vID[11], *graph);
    EdgeID e14;
    boost::tie(e14, ok) = boost::add_edge(vID[11], vID[12], *graph);
    EdgeID e15;
    boost::tie(e15, ok) = boost::add_edge(vID[3], vID[4], *graph);
    EdgeID e16;
    boost::tie(e16, ok) = boost::add_edge(vID[4], vID[5], *graph);
    EdgeID e17;
    boost::tie(e17, ok) = boost::add_edge(vID[5], vID[6], *graph);
    EdgeID e18;
    boost::tie(e18, ok) = boost::add_edge(vID[6], vID[3], *graph);
 
 
 


	visitorTraversal* vis = new visitorTraversal();
	visitorTraversal* vis2 = new visitorTraversal();
	visitorTraversal* vis3 = new visitorTraversal();
	visitorTraversal* vis4 = new visitorTraversal();
    vis->pths=pth;
    vis2->pths=pth;
    vis3->pths=pth;
    vis4->pths=pth;
    //std::cout << "vis" << std::endl;
    vis->constructPathAnalyzer(graph,false, vID[0], vID[12]);
    //std::cout << "pths: " << std::endl;
//    for (int i = 0; i < vis->pths.size(); i++) {
//		std::cout << "path" << i << ": " << std::endl;
//		for (int j = 0; j < vis->pths[i].size(); j++) {
//	        std::cout << vis->pths[i][j] << ", ";
//	    }
//	    std::cout << std::endl;
//	}
//	std::cout << std::endl;
  //  std::cout << "vis2" << std::endl;
    vis2->constructPathAnalyzer(graph, false,vID[7], vID[11]);
 /*   for (int i = 0; i < vis2->pths.size(); i++) {
		std::cout << "path" << i << ": " << std::endl;
		for (int j = 0; j < vis2->pths[i].size(); j++) {
	        std::cout << vis2->pths[i][j] << ", ";
	    }
	    std::cout << std::endl;
	}
	std::cout << std::endl;
    std::cout << "vis3" << std::endl;
*/
    vis3->constructPathAnalyzer(graph, false, vID[11], vID[12]);
/*
    for (int i = 0; i < vis3->pths.size(); i++) {
		std::cout << "path" << i << ": " << std::endl;
		for (int j = 0; j < vis3->pths[i].size(); j++) {
	        std::cout << vis3->pths[i][j] << ", ";
	    }
	    std::cout << std::endl;
	}
	std::cout << std::endl;
    std::cout << "vis4" << std::endl;
*/
    vis4->constructPathAnalyzer(graph,false,vID[3], vID[3]);
/*    for (int i = 0; i < vis4->pths.size(); i++) {
		std::cout << "path" << i << ": " << std::endl;
		for (int j = 0; j < vis4->pths[i].size(); j++) {
	        std::cout << vis4->pths[i][j] << ", ";
	    }
	    std::cout << std::endl;
	}
	std::cout << std::endl;
*/
	return 0;
	
}  
