#include <rose.h>
#include <stdio.h>
#include <iostream>
//#include <mysql.h>
#include <string>

using namespace std;
#define debug 1

#include <GL/glut.h>

#include <math.h>
#include "helper.h"

static bool debug_me = true;

#define DELTA 5
int x = 0;
int rotateX = 0;
int y = 0;
int rotateY = 0;
int z = 0;
int rotateZ = 0;
int speed = 0;
int u, v;

#define SAVE 0
#define LOAD 0
#define FILE2 0

GLfloat *****pts;

static void 
display(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

  glCallList(1);
  glutSwapBuffers();
}

void initGL(int argc, char** argv) {
  /************** INITGL *************************/
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
  /************** INITGL *************************/

}

void 
loadFile( const char* filename, unsigned int maxX, unsigned int maxY, int max, 
	 unsigned int& pointsX, unsigned int& pointsY) {
  /************** LOAD *************************/  
    //#if LOAD
  // readFromFile

  cerr << "Loading " << filename <<".\n";
  ifstream myfile (filename);//+".coord");
  if (myfile.is_open())   {
    myfile >> maxX;
    myfile >> maxY;
  }

  pointsX =maxX/max;
  pointsY =maxY/max;
  cout << "maxX:"<<maxX << "  pointsX="<<pointsX << endl;
  cout << "maxY:"<<maxY << "  pointsY="<<pointsY << endl;

  /************** 2FILES ***********/  
#if FILE2
  GLfloat pts2[pointsX+1][pointsY+1][max][max][3];
#else
  //  GLfloat pts[pointsX+1][pointsY+1][max][max][3];
  pts= new GLfloat****[pointsX+1];
  for (unsigned int i=0; i<(pointsX+1);++i) {
    pts[i] = new GLfloat***[pointsY+1];
    for (unsigned int ii=0; ii<(pointsY+1);++ii) {
      pts[i][ii] = new GLfloat**[max];
      for (int iii=0; iii<max;++iii) {
	pts[i][ii][iii] = new GLfloat*[max];
	for ( int iiii=0; iiii<max;++iiii) {
	  pts[i][ii][iii][iiii] = new GLfloat[3];
	}
      }
    }
  }


#endif
  /************** 2FILES ***********/  

  if (myfile.is_open())   {

    unsigned int x=0;
    unsigned int y=0;
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

  /************** 2FILES ***********/  
#if FILE2
      pts2[fieldX][fieldY][offsetX][offsetY][0] =line1;
      pts2[fieldX][fieldY][offsetX][offsetY][1] =line2;
      pts2[fieldX][fieldY][offsetX][offsetY][2] =line3;
#else
      pts[fieldX][fieldY][offsetX][offsetY][0] =line1;
      pts[fieldX][fieldY][offsetX][offsetY][1] =line2;
      pts[fieldX][fieldY][offsetX][offsetY][2] =line3;
#endif
  /************** 2FILES ***********/  

      y++; 
    }
    myfile.close();
  }
  else cout << "Unable to open file"; 

  cerr << "Done loading a file.\n";


  /************** 2FILES ***********/    
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
  /************** 2FILES ***********/  

  /************** LOAD CONT *************************/  
  //#else



  GLfloat maxHeightPatch[pointsX][pointsY];
  for (unsigned int x=0; x<maxX;x++) {
    for (unsigned int y=0; y<maxY;y++) {
      int fieldX = x/max;
      int fieldY =y/max;
      maxHeightPatch[fieldX][fieldY] = 0;
    }
  }

}

void calculate(FunctionType& functions, unsigned int maxX, unsigned int maxY, int max,
	       unsigned int& pointsX, unsigned int& pointsY) {

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

  
  pointsX =maxX/max;
  pointsY =maxY/max;
  cerr << " Initializing fields in x = " <<pointsX<<"  y = " <<pointsY<<endl;

  
  GLfloat factor = 2.0f;
  //GLfloat max_f =(float) -(max-1);

  //  GLfloat pts[pointsX][pointsY][max][max][3];
  pts= new GLfloat****[pointsX+1];
  for (unsigned int i=0; i<(pointsX+1);++i) {
    pts[i] = new GLfloat***[pointsY+1];
    for (unsigned int ii=0; ii<(pointsY+1);++ii) {
      pts[i][ii] = new GLfloat**[max];
      for (int iii=0; iii<max;++iii) {
	pts[i][ii][iii] = new GLfloat*[max];
	for (int iiii=0; iiii<max;++iiii) {
	  pts[i][ii][iii][iiii] = new GLfloat[3];
	  //cerr << " creating : pts["<<i<<"]["<<ii<<"]["<<iii<<"]["<<iiii<<"][3]"<<endl;
	}
      }
    }
  }
  cerr << " Done creating 5 Dim DB." << endl;
  if (debug_me) {
    ROSE_ASSERT(pointsX==5);
    ROSE_ASSERT(pointsY==5);
    ROSE_ASSERT(max==8);
    ROSE_ASSERT(maxX==47);
    ROSE_ASSERT(maxY==47);
  }
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
#if 0
      cerr << " factor : " << factor << "   pointsX : " << pointsX << "   pointsY : " << pointsY << "  max : " << max <<endl;
      cerr << " fieldX : " << fieldX << "   offsetX : " << offsetX << endl;
      cerr << " fieldY : " << fieldY << "   offsetY : " << offsetY << endl;
      cerr << " alloc : pts[pointX][pointsY][max][max][3]" <<  endl;
      cerr << " refer : pts[fieldX][fieldY][offsetX][offsetY][0]" << endl;
      cerr << " total : " << (factor*((GLfloat)offsetX+((max-1)*fieldX))) << endl;
#endif
      pts[fieldX][fieldY][offsetX][offsetY][0]=factor*((GLfloat)offsetX+((max-1)*fieldX));
      pts[fieldX][fieldY][offsetX][offsetY][1]=factor*((GLfloat)offsetY+((max-1)*fieldY));
      pts[fieldX][fieldY][offsetX][offsetY][2]=input[x][y][0];
      //      cerr << " result ======== " << pts[fieldX][fieldY][offsetX][offsetY][0] << endl;
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

  //#endif // LOAD
  
  /************** LOAD *************************/  

  GLfloat maxHeightPatch[pointsX][pointsY];
  for (unsigned int x=0; x<maxX;x++) {
    for (unsigned int y=0; y<maxY;y++) {
      int fieldX = x/max;
      int fieldY =y/max;
      maxHeightPatch[fieldX][fieldY] = 0;
    }
  }


    //#if LOAD
    //#else
  /************** NOLOAD *************************/  

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
	      if (coord_x<0 || coord_x>(int)maxX || coord_y<0 || coord_y>(int)maxY) continue;
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
	    //GLfloat current = pts[fieldX_w][fieldY_w][offsetX_w][offsetY_w][2];
	    //	    if (fieldX==1 && fieldY==2 && height>65 )    
	    //      cerr << " coordx="<<coord_x<<" coordy="<<coord_y<<"   fieldX_w="<<fieldX_w<<" fieldY_w="<<fieldY_w<<
	    //" offsetX_w="<<offsetX_w<<" offsetY_w="<<offsetY_w<<endl;
	    if (coord_x<0 || coord_x>(int)maxX || coord_y<0 || coord_y>(int)maxY) continue;
	    bool center=false;
	    for (int k=-1;k<2;k++)
	      for (int m=-1;m<2;m++)
		if ((coord_x+k)==(int)x && (coord_y+m)==(int)y)
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


  //#endif // LOAD

  /************** NOLOAD *************************/  


}


void 
render(unsigned int pointsX, unsigned int pointsY, int nrknots, int max) {


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
      //GLfloat height = maxHeightPatch[x][y];

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
}


void saveFile(GLfloat *****pts, const char* filename, unsigned int maxX, unsigned int maxY, int max, 
	 unsigned int& pointsX, unsigned int& pointsY) {
  /************** SAVE *************************/  
    //#if SAVE
  // printToFile
  ofstream myfile;
  myfile.open (filename);//+".coord");
  cerr << "Writing this to a file.\n";
  myfile << maxX <<endl;
  myfile << maxY <<endl;
  if (debug_me) {
    ROSE_ASSERT(pointsX==5);
    ROSE_ASSERT(pointsY==5);
    ROSE_ASSERT(max==8);
    ROSE_ASSERT(maxX==47);
    ROSE_ASSERT(maxY==47);
  }

  for (unsigned int x=0; x<maxX;x++) {
    for (unsigned int y=0; y<maxY;y++) {
      int fieldX = x/max;
      int offsetX = x%max;
      int fieldY =y/max;
      int offsetY = y%max;
      //GLfloat height = pts[fieldX][fieldY][offsetX][offsetY][2];
      myfile << pts[fieldX][fieldY][offsetX][offsetY][0] << endl;
      myfile << pts[fieldX][fieldY][offsetX][offsetY][1] << endl;
      myfile << pts[fieldX][fieldY][offsetX][offsetY][2] << endl;
    }
  }    
  myfile.close();
  cerr << "Done writing this to a file.\n";
  exit(0);
  //#else

}

void displayAll(FunctionType& functions,  unsigned int maxX,
		unsigned int maxY, int argc, char** argv, int nrknots, int max,
		std::string filen, std::string filen2, bool load, bool save,
		unsigned int& pointsX, unsigned int& pointsY) 
{

  string filenC = filen+".coord";
  string filen2C = filen2+".coord";
  const char* filename = filenC.c_str();
  const char* filename2 = filen2C.c_str();

  /************** PRINTOUT *************************/
#if FILE2
  cerr << " loading " << filename << " and " << filename2 << endl;
#else
  cerr << " loading " << filename << endl;
#endif

  if (!load)
    cerr << "Building array - functions: " << functions.size() <<
      "  maxX:"<<maxX<<"  maxY:"<<maxY<<endl;
  /************** PRINTOUT *************************/

  initGL(argc, argv);

  pts=NULL;
  if (load)
    loadFile(filename, maxX, maxY, max, pointsX, pointsY);
  else 
    calculate(functions, maxX, maxY, max, pointsX, pointsY);

  if (debug_me) {
    ROSE_ASSERT(pointsX==5);
    ROSE_ASSERT(pointsY==5);
    ROSE_ASSERT(max==8);
    ROSE_ASSERT(maxX==47);
    ROSE_ASSERT(maxY==47);
  }

  
  ROSE_ASSERT(pts!=NULL);

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


  //render(pointsX, pointsY, nrknots, max);

  if (save) {
    saveFile(pts, filename, maxX, maxY, max, pointsX, pointsY);
    return;
  }

  delete[] pts;
  pts=NULL;

  glutDisplayFunc(display);
  glutMainLoop();
}


SgProject* 
parseBinaryFile(std::string name) {
  // binary code analysis *******************************************************
  //  cerr << " Starting binary analysis ... " << endl;
  //RoseBin_Def::RoseAssemblyLanguage = RoseBin_Def::x86;
  //fprintf(stderr, "Starting binCompass frontend...\n");
  char* nameChar = &name[0];
  char* argv[] = {"vizzBinary",nameChar};
  SgProject* project = frontend(2,argv);
  ROSE_ASSERT (project != NULL);
  SgAsmFile* file = project->get_file(0).get_binaryFile();
  const SgAsmInterpretationPtrList& interps = file->get_interpretations();
  ROSE_ASSERT (interps.size() == 1);
  SgAsmInterpretation* interp = interps[0];

  string fname = StringUtility::stripPathFromFileName(name);

  // control flow analysis  *******************************************************
  bool forward = true;
  bool edges = true;
  bool mergedEdges = false;
  VirtualBinCFG::AuxiliaryInformation* info = new VirtualBinCFG::AuxiliaryInformation(project);
  RoseBin_DotGraph* dotGraph = new RoseBin_DotGraph(info);
  //RoseBin_GMLGraph* gmlGraph = new RoseBin_GMLGraph(info);
  string cfgFileName = fname+"-cfg.dot";
  cerr << " Creating CFG : " << cfgFileName << endl;
  RoseBin_ControlFlowAnalysis* cfganalysis = new RoseBin_ControlFlowAnalysis(interp->get_global_block(), forward, new RoseObj(), edges, info);
  cfganalysis->run(dotGraph, cfgFileName, mergedEdges);

  cerr << " Creating assembly : " << fname+"-unp.s" << endl;
  unparseAsmStatementToFile( fname+"-unp.s", interp->get_global_block());
  return project;
}


int main(int argc, char** argv) {
  int nrknots=16;
  int max =8;

  if (argc < 2) {
    fprintf(stderr, "Usage: %s [executableName|-dir dirname] [OPTIONS]\n", argv[0]);
    cout << "\nOPTIONS: " <<endl;
    cout << "-save             - run all binaries and save footprints. " << endl; 
    cout << "-load             - load all footprints and run analyses. " << endl; 
    return 1;
  }
  string execName = argv[1];
  string dir ="";
  if (execName=="-dir")
    dir=argv[2];

  // create out folder
  string filenameDir="out";
  mode_t mode = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH;
  mkdir(filenameDir.c_str(), mode);

  bool save = false;
  if (containsArgument(argc, argv, "-save")) {
    save = true;
  }
  bool load = false;
  if (containsArgument(argc, argv, "-load")) {
    load = true;
    save=false;
  }

  string def_db_name    ="kleza";
  string def_db_name2    ="klezb";

  vector<string> files = vector<string>();
  if (dir!="") {
    cout << "Loading Binaries at : " << dir << endl;
    getdir(dir,files);
  }  else {
    cout << "Loading file : " << execName << endl;
    files.push_back(execName);
  }
  for (unsigned int i = 0;i < files.size();i++) {
    string name = files[i];
    cout << "Adding Binary : " << name << endl;
  }

  SgProject* project = NULL;
  unsigned int pointsX =0;
  unsigned int pointsY =0;

  if (save) {
    // load all binaries iteratively and save it as footprint
    cout << "... Saving all binaries to footprint." << endl;
    for (unsigned int i = 0;i < files.size();i++) {
      string name = files[i];
      cout << "\nAnalysing Binary : " << name << endl;
      project= parseBinaryFile(name);
      Traversal trav;
      trav.run(project,max);
      unsigned int maxX = trav.maxX+1;
      unsigned int maxY = trav.maxY+1;

      cout << "Saving Binary Footprint : " << name << endl;
      displayAll(trav.functions, maxX, maxY, argc, argv, nrknots, max, def_db_name, def_db_name2, load, save, pointsX, pointsY);
    }
  } else if (load) {
    // load all footprints at once and run analyses
    cout << "... Loading all footprints in out directory." << endl;
    for (unsigned int i = 0;i < files.size();i++) {
      string name = files[i];
      cout << "\nLoading Binary and Visualizing footprint: " << name << endl;
    }
  } else {
    // preform simple analysis on current file(s)
    cout << "... Running footprint analyses without load/save." << endl;
    for (unsigned int i = 0;i < files.size();i++) {
      string name = files[i];
      cout << "\nAnalysing Binary : " << name << endl;
      project = parseBinaryFile(name);
      Traversal trav;
      trav.run(project,max);
      unsigned int maxX = trav.maxX+1;
      unsigned int maxY = trav.maxY+1;

      cout << "Visualizing Binary Footprint : " << name << endl;
      displayAll(trav.functions, maxX, maxY, argc, argv, nrknots, max, def_db_name, def_db_name2, load, save, pointsX, pointsY);
    }
  }


#if LOAD
  Traversal trav;
  displayAll(trav.functions, 0, 0, argc, argv, nrknots, max, def_db_name, def_db_name2, load, save, pointsX, pointsY);
#else

  

#endif
  return 0;
}
