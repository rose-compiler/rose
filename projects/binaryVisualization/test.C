#include <rose.h>
#include <stdio.h>
#include <iostream>
#include <string>



#define debug 1

#include <GL/glut.h>
#include <GL/freeglut.h> 

#include <math.h>
#include "helper.h"

#include "boost/multi_array.hpp"
#include "boost/filesystem/operations.hpp" // includes boost/filesystem/path.hpp


using namespace std;
using namespace boost::filesystem;
using namespace boost;

static bool debug_me = false;

#define DELTA 5
int x = 0;
int rotateX = 0;
int y = 0;
int rotateY = 0;
int z = 0;
int rotateZ = 0;
int speed = 0;
int u, v;


bool nextKey=false;

typedef boost::multi_array<GLfloat, 5> array_type;
array_type pts(boost::extents[0][0][0][0][3]);

//array_type pts2(boost::extents[0][0][0][0][3]);


class Element {
public:
  Element(string name, unsigned int maxX, unsigned int maxY, unsigned int pointsX, unsigned int pointsY):name(name),maxX(maxX),maxY(maxY),pointsX(pointsX),pointsY(pointsY){};
  string name;
  unsigned int maxX;
  unsigned int maxY;
  unsigned int pointsX;
  unsigned int pointsY;
};

std::map <std::string, Element*> elements;
std::map <std::string, array_type*> arrays;

int callList = 0;

static void 
display(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

  for (int i=1; i<=callList;++i)
    if (i>(callList-4))
      glCallList(i);
#if 0
  glutSwapBuffers();
#endif
}

Element* getElement(std::string name) {
  std::map <std::string, Element*>::const_iterator el=  elements.find(name);
  Element* element=NULL;
  if (el!=elements.end())
    element=el->second;
  return element;
}

array_type* getArray(std::string name) {
  std::map <std::string, array_type*>::const_iterator ar= arrays.find(name);
  array_type* array = NULL;
  if (ar!=arrays.end())
    array=ar->second;
  return array;
}



void idle() {

}

void key(unsigned char k, int x, int y)
{
  if (k == 27)
    {
      exit(0);
    }
}


void specialkey(int k, int x, int y)
{
  if (k == GLUT_KEY_LEFT)
    ;
  else if (k == GLUT_KEY_RIGHT) {
    cerr << " right key pressed " << endl;
    nextKey=true;
  }
  else if (k == GLUT_KEY_UP)
    ;
  else if (k == GLUT_KEY_DOWN)
    ;
  else if (k == GLUT_KEY_HOME)
    ;
  else if (k == GLUT_KEY_END)
    ;
  else if (k == GLUT_KEY_PAGE_UP)
    ;
  else if (k == GLUT_KEY_PAGE_DOWN)
    ;
}



int initGL(string name,int pX, int pY) {
  /************** INITGL *************************/
  // store height and weight
  int id=0;
#if 0
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(800, 800);
  id=glutCreateWindow(name.c_str());
  glutPositionWindow(pX,pY);


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


  glutDisplayFunc(display);
  glutKeyboardFunc(key);
  glutSpecialFunc(specialkey);
  glutIdleFunc(idle);
#endif
  return id;
}







void widenFields(unsigned int maxX, unsigned int maxY, int max,
                 unsigned int pointsX, unsigned int pointsY) {

  GLfloat maxHeightPatch[pointsX][pointsY];
  for (unsigned int x=0; x<maxX;x++) {
    for (unsigned int y=0; y<maxY;y++) {
      int fieldX = x/max;
      int fieldY =y/max;
      maxHeightPatch[fieldX][fieldY] = 0;
    }
  }


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
      assert (fieldX >= (int)0 && fieldX < (int)pointsX);
      assert (fieldY >= (int)0 && fieldY < (int)pointsY);
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
	      if (coord_x<0 || coord_x>=(int)maxX || coord_y<0 || coord_y>=(int)maxY) continue;
	      assert (fieldX_w>=0 && fieldX_w<(int)pointsX);
	      assert (fieldY_w>=0 && fieldY_w<(int)pointsY);
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
	    if (coord_x<0 || coord_x>=(int)maxX || coord_y<0 || coord_y>=(int)maxY) continue;
	    bool center=false;
	    for (int k=-1;k<2;k++)
	      for (int m=-1;m<2;m++)
		if ((coord_x+k)==(int)x && (coord_y+m)==(int)y)
		  center = true;
		
	    float distance = sqrt((coord_x-x)*(coord_x-x)+(coord_y-y)*(coord_y-y));
	    if (fieldX_w!=fieldX || fieldY_w!=fieldY ) 
	      if (!center) {
		float inc = localMax/0.2/(distance*distance*distance*distance);
		assert (fieldX_w>=0 && fieldX_w<(int)pointsX);
		assert (fieldY_w>=0 && fieldY_w<(int)pointsY);
		if ((pts[fieldX_w][fieldY_w][offsetX_w][offsetY_w][2]+inc)<(maxHeight/2))
		  pts[fieldX_w][fieldY_w][offsetX_w][offsetY_w][2]+=inc;
	      }
	  } // for
	} // for

#endif

      } // if

    } // for
  } //for

}


void postProcess(unsigned int maxX,
                 unsigned int maxY, int max,
		 unsigned int& pointsX, unsigned int& pointsY) 
{


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
	assert ((fieldX - 1) >= (int)0 && (fieldX - 1) < (int)pointsX);
	assert (fieldY >= (int)0 );
	//cerr << " fieldY (y/max): " << fieldY << "  y:" << y << " max : " << max << "  pointsY : " << pointsY <<
	//      " maxY :" << maxY << endl; 
	assert (fieldY < (int)pointsY);
	GLfloat neighborHeight = pts[fieldX-1][fieldY][(max-1)][offsetY][2];
	
	if (neighborHeight>height) {
	  assert (fieldX >= (int)0 && fieldX < (int)pointsX);
	  assert (fieldY >= (int)0 && fieldY < (int)pointsY);
	  pts[fieldX][fieldY][offsetX][offsetY][2]=neighborHeight;
	}
	else if (height>neighborHeight) {
	  // right side
	  assert (fieldX - 1 >= (int)0 && fieldX - 1 < (int)pointsX);
	  assert (fieldY >= (int)0 && fieldY < (int)pointsY);
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
      assert (fieldX >= (int)0 && fieldX < (int)pointsX);
      assert (fieldY >= (int)0 && fieldY < (int)pointsY);
      GLfloat height = pts[fieldX][fieldY][offsetX][offsetY][2];
      // bottom
      if (offsetY==0 && fieldY!=0) {
	assert (fieldX >= (int)0 && fieldX < (int)pointsX);
	assert (fieldY-1 >= (int)0 && fieldY-1 < (int)pointsY);
	GLfloat neighborHeight = pts[fieldX][fieldY-1][offsetX][(max-1)][2];
	if (neighborHeight>height) {
	  pts[fieldX][fieldY][offsetX][offsetY][2]=neighborHeight;
	}
	else if (height>neighborHeight) {
	  assert (fieldX >= (int)0 && fieldX < (int)pointsX);
	  assert (fieldY-1 >= (int)0 && fieldY-1 < (int)pointsY);
	  // top side
	  pts[fieldX][fieldY-1][offsetX][(max-1)][2]=height;
	}
      }
    }
  } // adjust fields

  //  widenFields(maxX, maxY, max, pointsX, pointsY);

}

void postProcess(array_type* arr, unsigned int maxX,
                 unsigned int maxY, int max,
		 unsigned int& pointsX, unsigned int& pointsY) 
{

  ROSE_ASSERT(arr);
  // adjust field -- make neighbors go smooth together
  cerr << "Adjusting fields. " << endl;
  for (unsigned int x=0; x<maxX;x++) {
    for (unsigned int y=0; y<maxY;y++) {
      int fieldX = x/max;
      int offsetX = x%max;
      int fieldY =y/max;
      int offsetY = y%max;
      GLfloat height = (*arr)[fieldX][fieldY][offsetX][offsetY][2];

      // left
      if (offsetX==0 && fieldX!=0) {
	assert ((fieldX - 1) >= (int)0 && (fieldX - 1) < (int)pointsX);
	assert (fieldY >= (int)0 );
	//cerr << " fieldY (y/max): " << fieldY << "  y:" << y << " max : " << max << "  pointsY : " << pointsY <<
	//      " maxY :" << maxY << endl; 
	assert (fieldY < (int)pointsY);
	GLfloat neighborHeight = (*arr)[fieldX-1][fieldY][(max-1)][offsetY][2];
	
	if (neighborHeight>height) {
	  assert (fieldX >= (int)0 && fieldX < (int)pointsX);
	  assert (fieldY >= (int)0 && fieldY < (int)pointsY);
	  (*arr)[fieldX][fieldY][offsetX][offsetY][2]=neighborHeight;
	}
	else if (height>neighborHeight) {
	  // right side
	  assert (fieldX - 1 >= (int)0 && fieldX - 1 < (int)pointsX);
	  assert (fieldY >= (int)0 && fieldY < (int)pointsY);
	  (*arr)[fieldX-1][fieldY][(max-1)][offsetY][2]=height;
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
      assert (fieldX >= (int)0 && fieldX < (int)pointsX);
      assert (fieldY >= (int)0 && fieldY < (int)pointsY);
      GLfloat height = (*arr)[fieldX][fieldY][offsetX][offsetY][2];
      // bottom
      if (offsetY==0 && fieldY!=0) {
	assert (fieldX >= (int)0 && fieldX < (int)pointsX);
	assert (fieldY-1 >= (int)0 && fieldY-1 < (int)pointsY);
	GLfloat neighborHeight = (*arr)[fieldX][fieldY-1][offsetX][(max-1)][2];
	if (neighborHeight>height) {
	  (*arr)[fieldX][fieldY][offsetX][offsetY][2]=neighborHeight;
	}
	else if (height>neighborHeight) {
	  assert (fieldX >= (int)0 && fieldX < (int)pointsX);
	  assert (fieldY-1 >= (int)0 && fieldY-1 < (int)pointsY);
	  // top side
	  (*arr)[fieldX][fieldY-1][offsetX][(max-1)][2]=height;
	}
      }
    }
  } // adjust fields

  //  widenFields(maxX, maxY, max, pointsX, pointsY);

  cerr << " Fields adjusted " << endl;
}

void 
loadAllFiles( vector<string>& filesCoord, int max, 
	      unsigned int& maxX, unsigned int& maxY) {
  typedef array_type::index index;
  for (unsigned int i = 0;i < filesCoord.size();i++) {
    string name = filesCoord[i];
    unsigned int mX = 0;
    unsigned int mY = 0;
    /************** LOAD *************************/  
    const char* filename = name.c_str();
    cerr << "------ Loading " << name << endl;
    ifstream myfile (filename);//+".coord");
    if (myfile.is_open())   {
      myfile >> mX;
      myfile >> mY;
    }

    if (mX>maxX) maxX=mX;
    if (mY>maxY) maxY=mY;
  }
  unsigned int pointsX = 0;
  unsigned int pointsY = 0;
  pointsX =(maxX+max-1)/max;
  pointsY =(maxY+max-1)/max;

  cerr << "\nTOTAL  -- maxX: " << maxX << "  maxY: " << maxY << "   pointsX: " << pointsX << "  pointsY: " << pointsY << endl << endl;
  for (unsigned int i = 0;i < filesCoord.size();i++) {
    string name = filesCoord[i];
    const char* filename = name.c_str();
    Element* el = new Element(filename,maxX,maxY,pointsX,pointsY);
    elements[filename]=el;

    array_type* pts3 = new array_type(boost::extents[pointsX][pointsY][max][max][3]);
    for(index i = 0; i != pointsX; ++i)
      for(index j = 0; j != pointsY; ++j)
	for(index k = 0; k != max; ++k)
	  for(index l = 0; l != max; ++l)
	    for(index m = 0; m != 3; ++m)
	      (*pts3)[i][j][k][l][m] = 0;

    arrays[filename]=pts3;
  }  

  Element* element=NULL;
  array_type* array=NULL;
  cerr << "------ Filling elements " << endl;
  for (unsigned int i = 0;i < filesCoord.size();i++) {
    string name = filesCoord[i];
    element = getElement(name);
    array = getArray(name);
    const char* filename = name.c_str();
    ifstream myfile (filename);//+".coord");

    unsigned int tmpX;
    unsigned int tmpY;
    if (myfile.is_open())   {
      myfile >> tmpX;
      myfile >> tmpY;
    }

    if (myfile.is_open())   {
      unsigned int x=0;
      unsigned int y=0;
      GLfloat line1;
      GLfloat line2;
      GLfloat line3;
      while (! myfile.eof() )  {
	if (y>=tmpY) {
	  y=0; x++;
	  if (x==tmpX) break;
	}
	int fieldX = x/max;
	int offsetX = x%max;
	int fieldY =y/max;
	int offsetY = y%max;
	myfile >> line1;
	myfile >> line2;
	myfile >> line3;
	if (fieldX >= (int)0 && fieldX < (int)pointsX && fieldY >= (int)0 && fieldY < (int)pointsY) {
	  (*array)[fieldX][fieldY][offsetX][offsetY][0] =line1;
	  (*array)[fieldX][fieldY][offsetX][offsetY][1] =line2;
	  (*array)[fieldX][fieldY][offsetX][offsetY][2] =line3;
	}
	y++; 
      }
      myfile.close();
    }
    else cout << "Unable to open file"; 
  }
  cerr << "Done filling elements." << endl;;

}





void 
loadFile( string filenC, unsigned int &maxX, unsigned int &maxY, int max, 
	  unsigned int& pointsX, unsigned int& pointsY) {
  /************** LOAD *************************/  
  const char* filename = filenC.c_str();
  cerr << "\n------ Loading " << filename << ".\n";
  ifstream myfile (filename);//+".coord");
  if (myfile.is_open())   {
    myfile >> maxX;
    myfile >> maxY;
  }
  pointsX =(maxX+max-1)/max;
  pointsY =(maxY+max-1)/max;

  cout << "maxX:"<<maxX << "  pointsX="<<pointsX << "  max: " << max << endl;
  cout << "maxY:"<<maxY << "  pointsY="<<pointsY << endl;

  typedef array_type::index index;
  pts.resize(boost::extents[pointsX][pointsY][max][max][3]);
  cout << " Alloc pts: " << pointsX << " " << pointsY << endl;
 
 
  cerr << " Filling pts with 0 ... " << pointsX << " " << pointsY << endl;
  for(index i = 0; i != pointsX; ++i) 
    for(index j = 0; j != pointsY; ++j)
      for(index k = 0; k != max; ++k)
	for(index l = 0; l != max; ++l)
	  for(index m = 0; m != 3; ++m)
	    pts[i][j][k][l][m] = 0;
  cerr << " Done Filling." << endl;

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
      //cout << endl << x <<" " << y << " "  << "  max: " <<max << endl;
      int fieldX = x/max;
      int offsetX = x%max;
      int fieldY =y/max;
      int offsetY = y%max;
      //cout  << fieldX<<" "<<fieldY<<" -- " <<offsetX<<" " << offsetY << endl;
      myfile >> line1;
      myfile >> line2;
      myfile >> line3;
      //cout  << line1<<" "<<line2<<" " <<line3 << endl;

 
      //      assert (fieldX >= (int)0 && fieldX < (int)pointsX);
      //assert (fieldY >= (int)0 && fieldY < (int)pointsY);
      if (fieldX >= (int)0 && fieldX < (int)pointsX && fieldY >= (int)0 && fieldY < (int)pointsY) {
	pts[fieldX][fieldY][offsetX][offsetY][0] =line1;
	pts[fieldX][fieldY][offsetX][offsetY][1] =line2;
	pts[fieldX][fieldY][offsetX][offsetY][2] =line3;
      }
      y++; 
    }
    myfile.close();
  }
  else cout << "Unable to open file"; 

  cerr << "Done loading a file.\n";

}



bool evaluateTwoFiles(array_type* arr1, array_type* arr2, array_type*& arr3, 
		      unsigned int maxX,
		      unsigned int maxY, int max,
		      unsigned int pointsX, unsigned int pointsY) {
 
  //  cerr << " Creating array : " << pointsX << " " << pointsY << endl;
  arr3 = new array_type(boost::extents[pointsX][pointsY][max][max][3]);
  int countNrOfHeightLarger50Percent=0;
  int totalFields=0;
  bool interesting=false;
  int totalBefore=0;
  int totalAfter=0;
  int totalMax=0;
  int outOfBounds=0;
  for (unsigned int x=0; x<maxX;x++) {
    for (unsigned int y=0; y<maxY;y++) {
      int fieldX = x/max;
      int offsetX = x%max;
      int fieldY =y/max;
      int offsetY = y%max;
      GLfloat max=0;
      GLfloat min=0;
      assert (fieldX >= (int)0 && fieldX < (int)pointsX);
      assert (fieldY >= (int)0 && fieldY < (int)pointsY);
      if ((*arr2)[fieldX][fieldY][offsetX][offsetY][2]>=(*arr1)[fieldX][fieldY][offsetX][offsetY][2]) {
	max = (*arr2)[fieldX][fieldY][offsetX][offsetY][2]; 
	min = (*arr1)[fieldX][fieldY][offsetX][offsetY][2]; 
      } else {
	max = (*arr1)[fieldX][fieldY][offsetX][offsetY][2]; 
	min = (*arr2)[fieldX][fieldY][offsetX][offsetY][2];
      }
      // if the diff is more than 50% of max then mark
      bool smaller = (max-min)<(max/2);
      //cerr << " max-min = " << (max-min) << " max/2 : " << (max/2) << "    " << smaller << endl;
      if (smaller)
	countNrOfHeightLarger50Percent++;
      totalFields++;
      (*arr3)[fieldX][fieldY][offsetX][offsetY][0] =(*arr1)[fieldX][fieldY][offsetX][offsetY][0]; 
      (*arr3)[fieldX][fieldY][offsetX][offsetY][1] =(*arr1)[fieldX][fieldY][offsetX][offsetY][1]; 
      (*arr3)[fieldX][fieldY][offsetX][offsetY][2] =max-min;
      totalBefore+=max+min;
      totalAfter+=(max-min);
      if (max>totalMax) totalMax=max;
      if ((max-min)>200)
	outOfBounds++;
    }
  }
  // if we have marked more than 50% of values
  ROSE_ASSERT(totalFields>0);
  double result = ((double)((double)countNrOfHeightLarger50Percent/(double)totalFields))*100;
  double result2 = 100-((double)((double)totalAfter/(double)totalBefore))*100;
  if (result>90 && result2>90 && outOfBounds<4)
    interesting=true;
  if (interesting )
  cerr << " Number of diffs>50% : " << countNrOfHeightLarger50Percent << "/" << totalFields << 
    "   in % : " << result << "    totals : " << totalAfter << "/" << totalBefore << "  in % : " << result2 << "   totalMax : " << totalMax << 
    "   outOfBounds : " << outOfBounds << endl;
  else
    if (debug_me)
  cout << " Number of diffs>50% : " << countNrOfHeightLarger50Percent << "/" << totalFields << 
    "   in % : " << result << "    totals : " << totalAfter << "/" << totalBefore << "  in % : " << result2 << "   totalMax : " << totalMax << 
    "   outOfBounds : " << outOfBounds << endl;

  return interesting;
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
  }
  cerr << "Done filling the input DB" << endl;


  pointsX =(maxX+max-1)/max;
  pointsY =(maxY+max-1)/max;
  cerr << " Initializing fields in x = " <<pointsX<<"  y = " <<pointsY<<endl;

  
  GLfloat factor = 2.0f;

  typedef array_type::index index;

  // Create a 3D array that is 3 x 4 x 2
  pts.resize(boost::extents[pointsX][pointsY][max][max][3]);

  for(index i = 0; i != pointsX; ++i) 
    for(index j = 0; j != pointsY; ++j)
      for(index k = 0; k != max; ++k)
	for(index l = 0; l != max; ++l)
	  for(index m = 0; m != 3; ++m)
	    pts[i][j][k][l][m] = 0;

  cerr << " Done creating 5 Dim DB." << endl;
  for (unsigned int x=0; x<maxX;x++) {
    for (unsigned int y=0; y<maxY;y++) {
      int fieldX = x/max;
      int offsetX = x%max;
      int fieldY =y/max;
      int offsetY = y%max;
      pts[fieldX][fieldY][offsetX][offsetY][0]=factor*((GLfloat)offsetX+((max-1)*fieldX));
      pts[fieldX][fieldY][offsetX][offsetY][1]=factor*((GLfloat)offsetY+((max-1)*fieldY));
      pts[fieldX][fieldY][offsetX][offsetY][2]=input[x][y][0];
    }
  }
  cerr << "Done initializing fields. " << endl;


  postProcess(maxX, maxY, max, pointsX, pointsY);
  widenFields(maxX, maxY, max, pointsX, pointsY);
}


void 
render(array_type* arr, unsigned int pointsX, unsigned int pointsY, int nrknots, int max) {
  glMatrixMode(GL_PROJECTION);
  gluPerspective(75.0, 1.0, 2.0, 5240.0);
  glMatrixMode(GL_MODELVIEW);
  glTranslatef(-55.0, -40.0, -85.0);
  //    glTranslatef(-218.0, -160.0, -315.0);
  glRotatef(330.0, 1.0, 0.0, 0.0);
  glRotatef(-35.0, 1.0, 0.0, 0.0);
  //    glRotatef(-30.0, 0.0, 0.0, 1.0);
  callList++;
  //  glNewList(1, GL_COMPILE);
  glNewList(callList, GL_COMPILE);
  /* Render red hill. */
  cerr << "Rendering... display list : " << callList << endl;


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
      if (arr==NULL)
	gluNurbsSurface(nurb, nrknots, knots, nrknots, knots,
			max * 3, 3, &pts[x][y][0][0][0],
			max, max, GL_MAP2_VERTEX_3);
      else
	gluNurbsSurface(nurb, nrknots, knots, nrknots, knots,
			max * 3, 3, &(*arr)[x][y][0][0][0],
			max, max, GL_MAP2_VERTEX_3);

      gluEndSurface(nurb);
    }
  }
  glEndList();
  display();
  //  display;
}


void saveFile(string filenC, unsigned int maxX, unsigned int maxY, int max, 
	      unsigned int& pointsX, unsigned int& pointsY) {
  /************** SAVE *************************/  
  //#if SAVE
  // printToFile
  const char* filename = filenC.c_str();
  ofstream myfile;
  myfile.open (filename);//+".coord");
  cerr << "Writing this to a file.\n";
  myfile << maxX <<endl;
  myfile << maxY <<endl;

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
}





void displayAll( array_type* arr, int nrknots, int max,
                 const unsigned int pointsX, const unsigned int pointsY) {
#if 0
  render(arr, pointsX, pointsY, nrknots, max);
  glutDisplayFunc(display);
#endif
}

void displayAll( int nrknots, int max,
                 const unsigned int pointsX, const unsigned int pointsY) {
  array_type* n=NULL;
#if 0
  render(n,pointsX, pointsY, nrknots, max);

  glutDisplayFunc(display);
  while (nextKey==false)
    glutMainLoopEvent();
  nextKey=false;
#endif
}



std::string ToUpper(std::string myString)
{
  const int length = myString.length();
  for(int i=0; i!=length ; ++i)
    {
      myString[i] = std::toupper(myString[i]);
    }
  return myString;
}


SgNode* disassembleFile(std::string tsv_directory, std::string& sourceFile){
  SgNode* globalBlock=NULL;
  int found = tsv_directory.rfind(".");
  string ending="";

  if (found!=string::npos) {
    ending =tsv_directory.substr(found+1,tsv_directory.length());
  }
  std::cout << "\nDisassembling: " << tsv_directory << " Ending : " << ending << std::endl;
  
  if(is_directory( tsv_directory  ) == true ){
    std::cout << "\nsql: " << tsv_directory << std::endl;
    RoseBin_Def::RoseAssemblyLanguage=RoseBin_Def::x86;
    RoseBin_Arch::arch=RoseBin_Arch::bit32;
    RoseBin_OS::os_sys=RoseBin_OS::linux_op;
    RoseBin_OS_VER::os_ver=RoseBin_OS_VER::linux_26;
    RoseFile* roseBin = new RoseFile( (char*)tsv_directory.c_str() );
    cerr << " ASSEMBLY LANGUAGE :: " << RoseBin_Def::RoseAssemblyLanguage << endl;
    // query the DB to retrieve all data
    globalBlock = roseBin->retrieve_DB();
    // traverse the AST and test it
    roseBin->test();
    sourceFile="false";
  } else if (ToUpper(ending)=="C" || ToUpper(ending)=="CPP" || ToUpper(ending)=="CXX") {
    cerr << "Found C code ... " << endl;
    vector<char*> args;
    args.push_back(strdup(""));
    args.push_back(strdup(tsv_directory.c_str()));
    args.push_back(0);
    globalBlock =  frontend(args.size()-1,&args[0]);
    sourceFile="true";
  }  else{
    vector<char*> args;
    args.push_back(strdup(""));
    args.push_back(strdup(tsv_directory.c_str()));
    args.push_back(0);
    
    ostringstream outStr; 
    for(vector<char*>::iterator iItr = args.begin(); iItr != args.end();
	++iItr )    {
      outStr << *iItr << " ";
    }     
    ;
    std::cout << "Calling " << outStr.str() << "  args: " << (args.size()-1) << std::endl;
    globalBlock =  frontend(args.size()-1,&args[0]);
    sourceFile="false";
  }
  return globalBlock;
};



SgProject* 
parseBinaryFile(std::string name) {
  // binary code analysis *******************************************************
  //  cerr << " Starting binary analysis ... " << endl;
  //RoseBin_Def::RoseAssemblyLanguage = RoseBin_Def::x86;
  //fprintf(stderr, "Starting binCompass frontend...\n");
  char* nameChar = &name[0];
  char* argv[] = {"vizzBinary",nameChar};
  //  SgProject* project = frontend(2,argv);
  string sourceFile="";
  SgNode* node = disassembleFile(name,sourceFile);
  if (node==NULL)
    return NULL;
  if (sourceFile=="true")
    return NULL;

  SgAsmBlock* globalBlock = isSgAsmBlock(node);
  SgProject* project = isSgProject(node);
  SgAsmInterpretation* interp=NULL;
  if (project==NULL) {
    cerr << " project == NULL :  " << node->class_name() << endl;
    return NULL;
  }
  
#if 0
  SgBinaryComposite* binFile = isSgBinaryComposite(project->get_fileList()[0]);
  if (binFile==NULL)
    return NULL;
  //  SgAsmFile* file = binFile->get_binaryFile();
  SgAsmGenericFile* file = binFile != NULL ? binFile->get_binaryFile() : NULL;


  interp = SageInterface::getMainInterpretation(file);
  //  ROSE_ASSERT (interps.size() == 1);
  //  SgAsmInterpretation* interp = interps[0];

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
  cfganalysis->setInitializedFalse();
  cfganalysis=NULL;
  delete cfganalysis;
#endif
  return project;
}




int main(int argc, char** argv) {
  int nrknots=16;
  int max =8;

  if (argc < 2) {
    fprintf(stderr, "Usage: %s [executableName|-dir dirname] [OPTIONS]\n", argv[0]);
    cout << "\nOPTIONS: " <<endl;
    cout << "-save             - run all binaries and save footprints. " << endl; 
    cout << "-load             - load one footprint and run analyses. " << endl; 
    cout << "-eval             - evaluate all footprints and run analyses. " << endl; 
    return 1;
  }
  string execName = argv[1];
  string dir ="";
  if (execName=="-dir")
    dir=argv[2];


  bool save = false;
  if (containsArgument(argc, argv, "-save")) {
    save = true;
  }
  bool load = false;
  if (containsArgument(argc, argv, "-load")) {
    load = true;
    save=false;
  }
  bool eval = false;
  if (containsArgument(argc, argv, "-eval")) {
    eval = true;
    load=false;
    save=false;
  }

  // string def_db_name    ="kleza";
  //string def_db_name2    ="klezb";

  vector<string> files = vector<string>();
  vector<string> filesBin = vector<string>();
  vector<string> filesCoord = vector<string>();
  if (dir!="") {
    cout << "Loading Binaries at : " << dir << endl;
    getdir(dir,files);
  }  else {
    filesBin.push_back(execName);
    execName+=".coord";
    cout << "Loading file : " << execName << endl;
    filesCoord.push_back(execName);
  }
  for (unsigned int i = 0;i < files.size();i++) {
    string name = files[i];
    string test="";
    string test2="";
    string test4="";
    if (name.size()>6)
      test=name.substr(name.size()-6,name.size());
    if (name.size()>2)
      test2=name.substr(name.size()-2,name.size());
    if (name.size()>4)
      test4=name.substr(name.size()-4,name.size());
    if (test==".coord") {
      filesCoord.push_back(name);
      cout << "Adding Binary Coord: " << name << endl;
    } else if (test2==".s" || test4==".dot") {
    } else {
      cout << "Adding Binary Bin: " << name << endl;
      filesBin.push_back(name);
    }
  }

  // create out folder
  string filenameDir="out";
  mode_t mode = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH;
  mkdir(filenameDir.c_str(), mode);

  SgProject* project = NULL;
  //  unsigned int pointsX =0;
  //unsigned int pointsY =0;
#if 0
  glutInit(&argc, argv);
#endif 
  if (save) {
    // load all binaries iteratively and save it as footprint
    cout << "... Saving all binaries to footprint." << endl;
    unsigned int pointsX =0;
    unsigned int pointsY =0;
    for (unsigned int i = 0;i < filesBin.size();i++) {
      string name = filesBin[i];
      initGL(name,0,0);
      cout << "\nAnalysing Binary (save): " << name << endl;
      project= parseBinaryFile(name);
      if (project==NULL)
	continue;
      Traversal trav;
      trav.run(project,max);
      unsigned int maxX = trav.maxX;
      unsigned int maxY = trav.maxY;
      string filenC = name+".coord";
      cout << "Saving Binary Footprint : " << filenC << endl;
      calculate(trav.functions, maxX, maxY, max, pointsX, pointsY);
      postProcess(maxX, maxY, max, pointsX, pointsY);
      saveFile(filenC, maxX, maxY, max, pointsX, pointsY);
    }
  } else if (load) {
    // load all footprints at once and run analyses
    unsigned int pointsX =0;
    unsigned int pointsY =0;
    cout << "... Loading all footprints in directory." << endl;
    for (unsigned int i = 0;i < filesCoord.size();i++) {
      string name = filesCoord[i];
      initGL(name,0,0);
      //      string filenC = name+".coord";
      cout << "\nLoading Binary and Visualizing footprint: " << name << endl;
      unsigned int maxX = 0;
      unsigned int maxY = 0;
      loadFile(name, maxX, maxY, max, pointsX, pointsY);
      postProcess(maxX, maxY,  max,  pointsX, pointsY);
      displayAll(nrknots, max, pointsX, pointsY);

    }
  } else if (eval && dir!="") { 
    // load all footprints at once and run analyses
    cout << "... Evaluating all footprints in  directory." << endl;
    unsigned int maxX=0;
    unsigned int maxY=0;
    loadAllFiles(filesCoord, max, maxX, maxY);
    unsigned int pointsX =(maxX+max-1)/max;
    unsigned int pointsY =(maxY+max-1)/max;
    Element* element=NULL;
    array_type* array=NULL;
    Element* element2=NULL;
    array_type* array2=NULL;
    array_type* arrayDiff=NULL;
    for (unsigned int i = 0;i < filesCoord.size();i++) {
      string name = filesCoord[i];
      element = getElement(name);
      array = getArray(name);
      if (element!=NULL && array!=NULL) {

	for (unsigned int k = (i+1);k < filesCoord.size();k++) {
	  string name2 = filesCoord[k];
	  if (debug_me)
	  cout << "\nEvaluating " << i << "/" << k << "/" << filesCoord.size()
               <<"  Binary and Visualizing footprint: " << name << " and " << name2 <<endl;
          if ((i%10)==0 && k==(i+1))
	  cerr << "\nEvaluating " << i << "/" << k << "/" << filesCoord.size()
               <<"  Binary and Visualizing footprint: " << name << " and " << name2 <<endl;
          element2 = getElement(name2);
	  array2 = getArray(name2);
	  if (element2!=NULL && array2!=NULL) {
	    bool interesting = evaluateTwoFiles(array,array2,arrayDiff,maxX, maxY,  max,  pointsX, pointsY);
	    ROSE_ASSERT(arrayDiff);
	    if ( interesting) {
	      int win0 = initGL(name+" "+name2,0,0);
	      postProcess(arrayDiff, maxX, maxY,  max,  pointsX, pointsY);
	      displayAll(arrayDiff, nrknots, max, pointsX, pointsY);
	    
	      int win1 = initGL(name,810,0);
	      postProcess(array, maxX, maxY,  max,  pointsX, pointsY);
	      displayAll(array, nrknots, max, pointsX, pointsY);

	      int win2 = initGL(name2,1620,0);
	      postProcess(array2, maxX, maxY,  max,  pointsX, pointsY);
	      displayAll(array2,nrknots, max, pointsX, pointsY);
#if 0
	      while (nextKey==false)
		glutMainLoopEvent();
	      nextKey=false;

	      glutDestroyWindow(win0);
	      glutDestroyWindow(win1);
	      glutDestroyWindow(win2);
#endif
	    }
	    delete arrayDiff;
	  }
	}
      }
    }
  } else {
    // preform simple analysis on current file(s)
    cout << "... Running footprint analyses without load/save." << endl;
    unsigned int pointsX =0;
    unsigned int pointsY =0;

    for (unsigned int i = 0;i < filesBin.size();i++) {
      string name = filesBin[i];
      initGL(name,0,0);
      cout << "\nAnalysing Binary (run): " << name << endl;
      project = parseBinaryFile(name);
      Traversal trav;
      trav.run(project,max);
      unsigned int maxX = trav.maxX;
      unsigned int maxY = trav.maxY;
      cout << "Visualizing Binary Footprint : " << name << endl;
      cout << " maxX = " << maxX << "  maxY = " << maxY << endl;
      calculate(trav.functions, maxX, maxY, max, pointsX, pointsY);
      postProcess(maxX, maxY,  max, pointsX, pointsY);
      displayAll(nrknots, max, pointsX, pointsY);
    }
  }
  cerr << " ALL DONE . " << endl;
  return 0;
}
