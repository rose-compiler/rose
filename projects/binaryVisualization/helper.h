

#ifndef __helper__
#define __helper__

#include <rose.h>

#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <string>
#include <iostream>
#include "ltdl.h"

#include <sys/stat.h>
#include <sys/types.h>

#define KEY_ESC 27
#define KEY_UP 101
#define KEY_DOWN 103
#define KEY_X 120
#define KEY_Y 121
#define KEY_Z 122

GLfloat mat_red_diffuse[] = { 0.7, 0.0, 0.1, 1.0 };
GLfloat mat_green_diffuse[] = { 0.0, 0.7, 0.1, 1.0 };
GLfloat mat_blue_diffuse[] = { 0.0, 0.1, 0.7, 1.0 };
GLfloat mat_yellow_diffuse[] = { 0.7, 0.8, 0.1, 1.0 };
GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat mat_shininess[] = { 100.0 };
//GLfloat knots[4] = { 0.0, 0.0, 1.0, 1.0};
//GLfloat knots[8] = { 0.0, 0.0, 0.0, 0.0,  1.0, 1.0, 1.0, 1.0 };
GLfloat knots[16] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 };
GLUnurbsObj *nurb;


// CH (4/30/2010): Since we use boost::unordered now, and boost provides hash function for pair<>,
// we don't need the following hash function any more.
/* 
namespace __gnu_cxx {
  template <> struct hash<std::pair<int,int> > {
    unsigned long operator()( const std::pair<int,int> &s) const {
      return (size_t) s.first;
    }
  };
}
*/

bool containsArgument(int argc, char** argv, std::string pattern) {
  for (int i = 2; i < argc ; i++) {
    if (argv[i]== pattern) {
      return true;
    }
  }
  return false;
}

int getdir (std::string dir, std::vector<std::string> &files)
{
  DIR *dp;
  struct dirent *dirp;
  if((dp  = opendir(dir.c_str())) == NULL) {
    std::cout << "Error(" << errno << ") opening " << dir << std::endl;
    return errno;
  }

  while ((dirp = readdir(dp)) != NULL) {
    std::string name = std::string(dirp->d_name);
    if (name!="." && name!="..") {
      files.push_back(name);
    }

  }
  closedir(dp);
  return 0;
}






class FunctionInfo {
public:
  int x;
  int y;
  int height;
  int weight;
  std::vector<std::string> names;
  void addName(std::string name) {names.push_back(name);}
  FunctionInfo(int x, int y, int height, int weight,
               std::string name):x(x),y(y),
                                 height(height), weight(weight)
  { addName(name);};

};


typedef rose_hash::unordered_map< std::pair<int,int>,
                                  FunctionInfo* > FunctionType;

class Traversal : public AstSimpleProcessing {

public:
  unsigned int maxX;
  unsigned int maxY;
  int divHeight;
  int maxim;
  FunctionType functions;

  void run(SgNode* node, int m) {
    maxX=0;
    maxY=0;
    divHeight=3;
    maxim=m;
    this->traverse(node,preorder);
  }
  void visit(SgNode* node) {
    SgAsmFunctionDeclaration* funcDecl =
      isSgAsmFunctionDeclaration(node);
    if (funcDecl) {
      std::cerr << " preprocessing function: " << funcDecl->get_name() << std::endl;
      std::vector<SgAsmStatement*> instructions;
      FindInstructionsVisitor vis;
      AstQueryNamespace::querySubTree(funcDecl, std::bind2nd( vis, &instructions ));
      //      vector<SgAsmStatement*> instructions= 
      //	funcDecl->get_statementList();
      int nrInst=0;
      unsigned int control=0;
      unsigned int call=0;
      std::vector<SgAsmStatement*>::iterator it= instructions.begin();
      for (;it!=instructions.end();it++) {
	SgAsmx86Instruction* inst = isSgAsmx86Instruction(*it);
	if (inst) {
	  nrInst++;
	  if (x86InstructionIsControlTransfer(inst) ||
	      x86InstructionIsConditionalFlagControlTransfer(inst) || 
	      x86InstructionIsConditionalControlTransfer(inst) ||
	      x86InstructionIsConditionalBranch(inst) ||
	      x86InstructionIsUnconditionalBranch(inst)
	      ) {
	    control++;
	  } else if (x86InstructionIsDataTransfer(inst) ||
                     x86InstructionIsConditionalFlagDataTransfer(inst) ||
		     x86InstructionIsConditionalDataTransfer(inst)
		     ) {
            call++;
	  }
	}
      } // for
      // another algo

      for (unsigned int u=0; u < funcDecl->get_name().size() ; u++) {
	char c = (funcDecl->get_name().c_str())[u];
	call += int(c); 
	control += int(c);
      }
      //	cerr << " call = " << call << endl;


                  call=(call%64);
      control=(control%64);

      if (call>maxY) maxY=call;
      if (control>maxX) maxX=control;

      // we should round maxX and maxY to a multiple of 8
      while (maxX%8!=0) maxX++;
      while (maxY%8!=0) maxY++;

      FunctionType::iterator fit= 
	functions.find(std::make_pair(control,call));
      if (fit==functions.end()) {
	// not found
	FunctionInfo* info = new FunctionInfo(control, call,
					      nrInst, 0, 
					      funcDecl->get_name());
	functions[std::make_pair(control,call)] = info;
	//cerr << "  .. creating : x="<<control<<" y="<<call
	//     << "  height="<<nrInst<<" weight=0"<<endl;
      } else {
	FunctionInfo* info = fit->second;
	info->height=info->height+nrInst;
	//	info->height=5;
	info->weight=info->weight+1;
	info->addName(funcDecl->get_name());
	functions[std::make_pair(control,call)] = info;
	//cerr << "  .. adding : x="<<control<<" y="<<call
	//     << "  height="<<info->height<<" weight="<<info->weight<<endl;
      }
    } // if
  }
};

#endif

