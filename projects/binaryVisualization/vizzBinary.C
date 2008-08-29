#include <rose.h>
#include <stdio.h>
#include <iostream>
//#include <mysql.h>
#include <string>

using namespace std;
#define debug 1

#include <GL/glut.h>

#include <math.h>

//#include "RoseBin.h"

namespace __gnu_cxx {
  template <> struct hash<std::pair<int,int> > {
    unsigned long operator()( const std::pair<int,int> &s) const {
      return (size_t) s.first;
    }
  };
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


typedef rose_hash::hash_map< pair<int,int>,
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
      cerr << " preprocessing function: " << funcDecl->get_name() <<endl;
      vector<SgAsmStatement*> instructions= 
	funcDecl->get_statementList();
      int nrInst=0;
      unsigned int control=0;
      unsigned int call=0;
      vector<SgAsmStatement*>::iterator it= instructions.begin();
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
      //      if (nrInst>0)
      //	nrInst =nrInst/divHeight;

      FunctionType::iterator fit= 
	functions.find(make_pair(control,call));
      if (fit==functions.end()) {
	// not found
	FunctionInfo* info = new FunctionInfo(control, call,
					      nrInst, 0, 
					      funcDecl->get_name());
	functions[make_pair(control,call)] = info;
	cerr << "  .. creating : x="<<control<<" y="<<call
	     << "  height="<<nrInst<<" weight=0"<<endl;
      } else {
	FunctionInfo* info = fit->second;
	info->height=info->height+nrInst;
	//	info->height=5;
	info->weight=info->weight+1;
	info->addName(funcDecl->get_name());
	functions[make_pair(control,call)] = info;
	cerr << "  .. adding : x="<<control<<" y="<<call
	     << "  height="<<info->height<<" weight="<<info->weight<<endl;
      }
    } // if
  }
};
 
#define KEY_ESC 27
#define KEY_UP 101
#define KEY_DOWN 103
#define KEY_X 120
#define KEY_Y 121
#define KEY_Z 122

#define DELTA 5
int x = 0;
int rotateX = 0;
int y = 0;
int rotateY = 0;
int z = 0;
int rotateZ = 0;
int speed = 0;


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

int u, v;

#define SAVE 0
#define LOAD 0
#define FILE2 0

static void 
display(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

  glCallList(1);
  glutSwapBuffers();
}





void displayAll(FunctionType& functions,  unsigned int maxX,
		unsigned int maxY, int argc, char** argv, int nrknots, int max,
		std::string filen, std::string filen2) 
{

  string filenC = filen+".coord";
  string filen2C = filen2+".coord";
  const char* filename = filenC.c_str();
  const char* filename2 = filen2C.c_str();

#if FILE2
  cerr << " loading " << filename << " and " << filename2 << endl;
#else
  cerr << " loading " << filename << endl;
#endif

#if LOAD
#else
  cerr << "Building array - functions: " << functions.size() <<
    "  maxX:"<<maxX<<"  maxY:"<<maxY<<endl;
#endif
  // store height and weight
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(800, 800);
  glutCreateWindow("Binary File Visualizer");


  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_AUTO_NORMAL);
  glEnable(GL_NORMALIZE);
  nurb = gluNewNurbsRenderer();
  gluNurbsProperty(nurb, GLU_SAMPLING_TOLERANCE, 25.0);
  gluNurbsProperty(nurb, GLU_DISPLAY_MODE, GLU_FILL);
  
#if LOAD
  // readFromFile

  cerr << "Loading " << filename <<".\n";
  ifstream myfile (filename);//+".coord");
    if (myfile.is_open())   {
      myfile >> maxX;
      myfile >> maxY;
    }

  int pointsX =maxX/max;
  int pointsY =maxY/max;
  cout << "maxX:"<<maxX << "  pointsX="<<pointsX << endl;
  cout << "maxY:"<<maxY << "  pointsY="<<pointsY << endl;

#if FILE2
  GLfloat pts2[pointsX+1][pointsY+1][max][max][3];
#else
  GLfloat pts[pointsX+1][pointsY+1][max][max][3];
#endif
    if (myfile.is_open())   {

      int x=0;
      int y=0;
      GLfloat line1;
      GLfloat line2;
      GLfloat line3;
	while (! myfile.eof() )  {
	  if (y>=maxY) {
	    y=0; x++;
	    if (x==maxX) break;
	  }
	  cout << endl << x <<" " << y << " "  << "  max: " <<max << endl;
	  int fieldX = x/max;
	  int offsetX = x%max;
	  int fieldY =y/max;
	  int offsetY = y%max;
	  cout  << fieldX<<" "<<fieldY<<" -- " <<offsetX<<" " << offsetY << endl;
	  myfile >> line1;
	  myfile >> line2;
	  myfile >> line3;
	  cout  << line1<<" "<<line2<<" " <<line3 << endl;

#if FILE2
	  pts2[fieldX][fieldY][offsetX][offsetY][0] =line1;
	  pts2[fieldX][fieldY][offsetX][offsetY][1] =line2;
	  pts2[fieldX][fieldY][offsetX][offsetY][2] =line3;
#else
	  pts[fieldX][fieldY][offsetX][offsetY][0] =line1;
	  pts[fieldX][fieldY][offsetX][offsetY][1] =line2;
	  pts[fieldX][fieldY][offsetX][offsetY][2] =line3;
#endif
	  y++; 
	}
	myfile.close();
      }
    else cout << "Unable to open file"; 

    cerr << "Done loading a file.\n";

  
#if FILE2

  cerr << "Loading " << filename2 <<".\n";
  ifstream myfile2 (filename2);//+".coord");
    if (myfile2.is_open())   {
      myfile2 >> maxX;
      myfile2 >> maxY;
    }

   pointsX =maxX/max;
   pointsY =maxY/max;
  cout << "maxX:"<<maxX << "  pointsX="<<pointsX << endl;
  cout << "maxY:"<<maxY << "  pointsY="<<pointsY << endl;

  GLfloat pts[pointsX+1][pointsY+1][max][max][3];
    if (myfile2.is_open())   {

      int x=0;
      int y=0;
      GLfloat line1;
      GLfloat line2;
      GLfloat line3;
	while (! myfile2.eof() )  {
	  if (y>=maxY) {
	    y=0; x++;
	  }
	  cout << endl << x <<" " << y << " "  << "  max: " <<max << endl;
	  int fieldX = x/max;
	  int offsetX = x%max;
	  int fieldY =y/max;
	  int offsetY = y%max;
	  if (x>=maxX) {
	    break;
	  };
	  
	  cout  << fieldX<<" "<<fieldY<<" -- " <<offsetX<<" " << offsetY << endl;
	  myfile2 >> line1;
	  myfile2 >> line2;
	  myfile2 >> line3;
	  cout  << line1<<" "<<line2<<" " <<line3 << endl;

	  GLfloat max=0;
	  GLfloat min=0;
	  if (pts2[fieldX][fieldY][offsetX][offsetY][2]>=line3) {
	    max = pts2[fieldX][fieldY][offsetX][offsetY][2]; min=line3;}
	  else {
	    max = line3; min = pts2[fieldX][fieldY][offsetX][offsetY][2];
	  }
	  pts[fieldX][fieldY][offsetX][offsetY][0] =line1;
	  pts[fieldX][fieldY][offsetX][offsetY][1] =line2;
	  pts[fieldX][fieldY][offsetX][offsetY][2] =max-min;

	  y++; 
	}
	myfile2.close();
      }
    else cout << "Unable to open file"; 

    cerr << "Done loading a file.\n";


#endif

#else
  GLfloat input[maxX][maxY][2];
  for (unsigned int x=0; x<maxX;x++) {
    for (unsigned int y=0; y<maxY;y++) {
      input[x][y][0]=0;
      input[x][y][1]=0;
    }
  }
  FunctionType::iterator it = functions.begin();
  for (;it!=functions.end();it++) {
    FunctionInfo* info = it->second;
    input[info->x][info->y][0]=info->height;
    input[info->x][info->y][1]=info->weight;
    if (debug) {
      cerr <<" input["<<info->x<<"]["<<info->y<<"][0]="<<info->height<<endl;
      cerr <<" input["<<info->x<<"]["<<info->y<<"][1]="<<info->weight<<endl<<endl;
    }
  }
  cerr << "Done filling the input DB" << endl;

  
  int pointsX =maxX/max;
  int pointsY =maxY/max;
  cerr << " Initializing fields in x = " <<pointsX<<"  y = " <<pointsY<<endl;

  
  float factor = 2.0f;
  GLfloat max_f =(float) -(max-1);

  GLfloat pts[pointsX][pointsY][max][max][3];
  for (unsigned int x=0; x<maxX;x++) {
    for (unsigned int y=0; y<maxY;y++) {
      int fieldX = x/max;
      int offsetX = x%max;
      int fieldY =y/max;
      int offsetY = y%max;
      if (debug) {
	if (input[x][y][0]>0) {
	  cerr << " x = "<<x<<"  y = " <<y<<endl;
	  cerr << " pts["<<fieldX<<"]["<<fieldY<<"]["<<offsetX<<"]["<<offsetY
	       << "][0]="<<factor*((GLfloat)offsetX+((max-1)*fieldX))<<endl;
	  cerr << " pts["<<fieldX<<"]["<<fieldY<<"]["<<offsetX<<"]["<<offsetY
	       << "][1]="<<factor*((GLfloat)offsetY+((max-1)*fieldY))<<endl;
	  cerr << " pts["<<fieldX<<"]["<<fieldY<<"]["<<offsetX<<"]["<<offsetY
	       << "][2]="<<input[x][y][0]<<endl<<endl;
	}
      }
      pts[fieldX][fieldY][offsetX][offsetY][0]=factor*((GLfloat)offsetX+((max-1)*fieldX));
      pts[fieldX][fieldY][offsetX][offsetY][1]=factor*((GLfloat)offsetY+((max-1)*fieldY));
      pts[fieldX][fieldY][offsetX][offsetY][2]=input[x][y][0];

    }
  }
  cerr << "Done initializing fields. " << endl;




#if 1
  // adjust field -- make neighbors go smooth together
  cerr << "Adjusting fields. " << endl;
  for (unsigned int x=0; x<maxX;x++) {
    for (unsigned int y=0; y<maxY;y++) {
      int fieldX = x/max;
      int offsetX = x%max;
      int fieldY =y/max;
      int offsetY = y%max;
      GLfloat height = pts[fieldX][fieldY][offsetX][offsetY][2];

      // left
      if (offsetX==0 && fieldX!=0) {
	GLfloat neighborHeight = pts[fieldX-1][fieldY][(max-1)][offsetY][2];
	
	if (neighborHeight>height) {
	  pts[fieldX][fieldY][offsetX][offsetY][2]=neighborHeight;
	}
	else if (height>neighborHeight) {
	  // right side
	  pts[fieldX-1][fieldY][(max-1)][offsetY][2]=height;
	}
      } 
    }
  }

  for (unsigned int x=0; x<maxX;x++) {
    for (unsigned int y=0; y<maxY;y++) {
      int fieldX = x/max;
      int offsetX = x%max;
      int fieldY =y/max;
      int offsetY = y%max;
      GLfloat height = pts[fieldX][fieldY][offsetX][offsetY][2];
      // bottom
      if (offsetY==0 && fieldY!=0) {
	GLfloat neighborHeight = pts[fieldX][fieldY-1][offsetX][(max-1)][2];
	if (neighborHeight>height) {
	  pts[fieldX][fieldY][offsetX][offsetY][2]=neighborHeight;
	}
	else if (height>neighborHeight) {
	  // top side
	  pts[fieldX][fieldY-1][offsetX][(max-1)][2]=height;
	}
      }
    }
  } // adjust fields

#endif

#endif // LOAD
  
  GLfloat maxHeightPatch[pointsX][pointsY];
  for (unsigned int x=0; x<maxX;x++) {
    for (unsigned int y=0; y<maxY;y++) {
      int fieldX = x/max;
      int fieldY =y/max;
      maxHeightPatch[fieldX][fieldY] = 0;
    }
  }
#if LOAD
#else

  // coloring
  for (unsigned int x=0; x<maxX;x++) {
    for (unsigned int y=0; y<maxY;y++) {
      int fieldX = x/max;
      int offsetX = x%max;
      int fieldY =y/max;
      int offsetY = y%max;
      GLfloat currentMax = maxHeightPatch[fieldX][fieldY];
      GLfloat currentHeight = pts[fieldX][fieldY][offsetX][offsetY][2];
      if (currentHeight>currentMax) 
        maxHeightPatch[fieldX][fieldY] = currentHeight;
      //	cerr << "x="<<x<<" y="<<y<<"  current="<<currentHeight<<" max="<<currentMax<<" new current= " << maxHeightPatch[fieldX][fieldY] << endl; 
    }
  }


  int maxHeight=50;
  // adjust field -- make neighbors go smooth together
  cerr << "Widening fields. " << endl;
  for (unsigned int x=0; x<maxX;x++) {
    for (unsigned int y=0; y<maxY;y++) {
      int fieldX = x/max;
      int offsetX = x%max;
      int fieldY =y/max;
      int offsetY = y%max;
      GLfloat height = pts[fieldX][fieldY][offsetX][offsetY][2];

      //      int diffHeight = (int) height-maxHeight;      
      if (height>maxHeight) {
	//	pts[fieldX][fieldY][offsetX][offsetY][2]=maxHeight;
      }

      // distribute diff over area
      int wideX =0;
      int wideY =0;
      int factor=5;
      if (offsetX==0 || offsetX==(max-1)) 
	wideX=(max-1)*factor; 
      else wideX=max*factor; 
      if (offsetY==0 || offsetY==(max-1)) 
	wideY=(max-1)*factor; 
      else wideY=max*factor; 

      GLfloat localMax = maxHeightPatch[fieldX][fieldY];

      if (localMax>0 && height>=(localMax-5) ) {
	//	pts[fieldX][fieldY][offsetX][offsetY][2]=0;
      int iteration=1;
      do {
	for (int i=-iteration;i<=iteration;i++) {
	  for (int j=-iteration;j<=iteration;j++) {
	    int coord_x = x+i;
	    int coord_y = y+j;
	    int fieldX_w = coord_x/max;
	    int offsetX_w = coord_x%max;
	    int fieldY_w = coord_y/max;
	    int offsetY_w = coord_y%max;
	    if (coord_x<0 || coord_x>maxX || coord_y<0 || coord_y>maxY) continue;
		if (pts[fieldX_w][fieldY_w][offsetX_w][offsetY_w][2]<maxHeight)
		    pts[fieldX_w][fieldY_w][offsetX_w][offsetY_w][2]+=0.2f;
	  }
	}
	  height = pts[fieldX][fieldY][offsetX][offsetY][2]/1.1;
	  pts[fieldX][fieldY][offsetX][offsetY][2]=height;
	  maxHeightPatch[fieldX][fieldY] = height;
	  iteration++;
	} while (height>maxHeight);

      

#if 1

	for (int i=-wideX;i<=wideX;i++) {
	  for (int j=-wideY;j<=wideY;j++) {
	    int coord_x = x+i;
	    int coord_y = y+j;
	    int fieldX_w = coord_x/max;
	    int offsetX_w = coord_x%max;
	    int fieldY_w = coord_y/max;
	    int offsetY_w = coord_y%max;
	    GLfloat current = pts[fieldX_w][fieldY_w][offsetX_w][offsetY_w][2];
	    //	    if (fieldX==1 && fieldY==2 && height>65 )    
	    //      cerr << " coordx="<<coord_x<<" coordy="<<coord_y<<"   fieldX_w="<<fieldX_w<<" fieldY_w="<<fieldY_w<<
	    //" offsetX_w="<<offsetX_w<<" offsetY_w="<<offsetY_w<<endl;
	    if (coord_x<0 || coord_x>maxX || coord_y<0 || coord_y>maxY) continue;
		bool center=false;
		for (int k=-1;k<2;k++)
		  for (int m=-1;m<2;m++)
		    if ((coord_x+k)==x && (coord_y+m)==y)
		      center = true;
		
		float distance = sqrt((coord_x-x)*(coord_x-x)+(coord_y-y)*(coord_y-y));
		if (fieldX_w!=fieldX || fieldY_w!=fieldY ) 
		  if (!center) {
		      float inc = localMax/0.2/(distance*distance*distance*distance);
		      if ((pts[fieldX_w][fieldY_w][offsetX_w][offsetY_w][2]+inc)<(maxHeight/2))
			  pts[fieldX_w][fieldY_w][offsetX_w][offsetY_w][2]+=inc;
		  }
		} // for
	  } // for

#endif

	} // if



      }
    }


#endif // LOAD

#if 1
  // adjust field -- make neighbors go smooth together
  cerr << "Adjusting fields. " << endl;
  for (unsigned int x=0; x<maxX;x++) {
    for (unsigned int y=0; y<maxY;y++) {
      int fieldX = x/max;
      int offsetX = x%max;
      int fieldY =y/max;
      int offsetY = y%max;
      GLfloat height = pts[fieldX][fieldY][offsetX][offsetY][2];

      // left
      if (offsetX==0 && fieldX!=0) {
	GLfloat neighborHeight = pts[fieldX-1][fieldY][(max-1)][offsetY][2];
	
	if (neighborHeight>height) {
	  pts[fieldX][fieldY][offsetX][offsetY][2]=neighborHeight;
	}
	else if (height>neighborHeight) {
	  // right side
	  pts[fieldX-1][fieldY][(max-1)][offsetY][2]=height;
	}
      } 
    }
  }

  for (unsigned int x=0; x<maxX;x++) {
    for (unsigned int y=0; y<maxY;y++) {
      int fieldX = x/max;
      int offsetX = x%max;
      int fieldY =y/max;
      int offsetY = y%max;
      GLfloat height = pts[fieldX][fieldY][offsetX][offsetY][2];
      // bottom
      if (offsetY==0 && fieldY!=0) {
	GLfloat neighborHeight = pts[fieldX][fieldY-1][offsetX][(max-1)][2];
	if (neighborHeight>height) {
	  pts[fieldX][fieldY][offsetX][offsetY][2]=neighborHeight;
	}
	else if (height>neighborHeight) {
	  // top side
	  pts[fieldX][fieldY-1][offsetX][(max-1)][2]=height;
	}
      }
    }
  } // adjust fields

#endif



    cerr << "Rendering..." << endl;


    glMatrixMode(GL_PROJECTION);
    gluPerspective(75.0, 1.0, 2.0, 5240.0);
    glMatrixMode(GL_MODELVIEW);
    glTranslatef(-40.0, -40.0, -70.0);
    //    glTranslatef(-218.0, -160.0, -315.0);
    glRotatef(330.0, 1.0, 0.0, 0.0);
    glRotatef(-35.0, 1.0, 0.0, 0.0);
    //    glRotatef(-30.0, 0.0, 0.0, 1.0);

    glNewList(1, GL_COMPILE);
    /* Render red hill. */

    for (unsigned int x=0;x<pointsX;x++) {
      for (unsigned int y=0;y<pointsY;y++) {
	GLfloat height = maxHeightPatch[x][y];

#if 1
	if ((y%2)==0 )
	  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_red_diffuse);
	else
	  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_green_diffuse);
#else
	  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_green_diffuse);
	/*  
	if (height<20)
	  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_green_diffuse);
	else if (height>=20 && height<49)
	  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_yellow_diffuse);
	else 
	  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_red_diffuse);
	*/
#endif
	gluBeginSurface(nurb);
	gluNurbsSurface(nurb, nrknots, knots, nrknots, knots,
			max * 3, 3, &pts[x][y][0][0][0],
			max, max, GL_MAP2_VERTEX_3);
	gluEndSurface(nurb);
      }
    }
    glEndList();


#if SAVE
    // printToFile
    ofstream myfile;
    myfile.open (filename);//+".coord");
    cerr << "Writing this to a file.\n";
    myfile << maxX <<endl;
    myfile << maxY <<endl;
    for (int x=0; x<maxX;x++) {
      for (int y=0; y<maxY;y++) {
	int fieldX = x/max;
	int offsetX = x%max;
	int fieldY =y/max;
	int offsetY = y%max;
	GLfloat height = pts[fieldX][fieldY][offsetX][offsetY][2];
	myfile << pts[fieldX][fieldY][offsetX][offsetY][0] << endl;
	myfile << pts[fieldX][fieldY][offsetX][offsetY][1] << endl;
	myfile << pts[fieldX][fieldY][offsetX][offsetY][2] << endl;
      }
    }    
    myfile.close();
    cerr << "Done writing this to a file.\n";
    exit(0);
#else

    glutDisplayFunc(display);
    glutMainLoop();
#endif
  }






  int main(int argc, char** argv) {
   int nrknots=16;
   int max =8;

  char* def_host_name  ="localhost"; 
  char* def_user_name  ="root";
  char* def_password   ="thomas";
  const char* socket = "/home/panas2/mysql/mysql.sock";

  //char* def_db_name    ="hellow_linux";


  //       char* def_db_name    ="scalpera";
      char* def_db_name    ="kleza";
  char* def_db_name2    ="klezb";
  //char* def_db_name    ="deborn";

  //char* def_db_name    ="tar114";
  //char* def_db_name    ="tar119";
  //char* def_db_name    ="hellow_linux2";
  //char* def_db_name    ="exploit";

#if LOAD
    Traversal trav;
    displayAll(trav.functions, 0, 0, argc, argv, nrknots, max, def_db_name, def_db_name2);
#else
  // binary code analysis *******************************************************
  cerr << " Starting binary analysis ... " << endl;
  RoseBin_Def::RoseAssemblyLanguage = RoseBin_Def::x86;
  fprintf(stderr, "Starting binCompass frontend...\n");
  SgProject* project = frontend(argc,argv);
  ROSE_ASSERT (project != NULL);
  SgAsmFile* file = project->get_file(0).get_binaryFile();
  const SgAsmInterpretationPtrList& interps = file->get_interpretations();
  ROSE_ASSERT (interps.size() == 1);
  SgAsmInterpretation* interp = interps[0];

#if 0

  RoseBin* roseBin = new RoseBin(def_host_name,
				def_user_name,
				def_password,
				def_db_name);

  RoseBin_Def::RoseAssemblyLanguage=RoseBin_Def::x86;
  RoseBin_Arch::arch=RoseBin_Arch::bit32;
  RoseBin_OS::os_sys=RoseBin_OS::linux_op;
  RoseBin_OS_VER::os_ver=RoseBin_OS_VER::linux_26;

  std::list<string> functionNames;

  //  RoseBin_Def::RoseAssemblyLanguage=arm;
  cerr << " ASSEMBLY LANGUAGE :: " << RoseBin_Def::RoseAssemblyLanguage << endl;
  // connect to the DB
  roseBin->connect_DB(socket);
  cerr << " Database open . " << endl;
  // allow to filter all but these functions
  roseBin->setFunctionFilter(functionNames);
  // query the DB to retrieve all data
  SgAsmNode* project = roseBin->retrieve_DB_IDAPRO();
  roseBin->close_DB();
  cerr << " Database closed . " << endl;

  // traverse the AST and test it
  roseBin->test();
#endif



    // control flow analysis  *******************************************************
    bool forward = true;
    bool edges = true;
    bool mergedEdges = false;
    VirtualBinCFG::AuxiliaryInformation* info = new VirtualBinCFG::AuxiliaryInformation(project);
    RoseBin_DotGraph* dotGraph = new RoseBin_DotGraph(info);
    RoseBin_GMLGraph* gmlGraph = new RoseBin_GMLGraph(info);
    char* cfgFileName = "cfg.dot";
    RoseBin_ControlFlowAnalysis* cfganalysis = new RoseBin_ControlFlowAnalysis(interp->get_global_block(), forward, new RoseObj(), edges, info);
    cfganalysis->run(dotGraph, cfgFileName, mergedEdges);

    //    RoseBin_unparse up;
    //up.init(project, "unparsed.s");
    //up.unparse();

    unparseAsmStatementToFile( "unparsed.s", interp->get_global_block());

    Traversal trav;
    trav.run(project,max);
    unsigned int maxX = trav.maxX+1;
    unsigned int maxY = trav.maxY+1;
  
    displayAll(trav.functions, maxX, maxY, argc, argv, nrknots, max, def_db_name, def_db_name2);
#endif
    return 0;
  }
