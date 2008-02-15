#ifndef IGES_READER_H
#define IGES_READER_H

#include "Mapping.h"

#include <time.h>
#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RECBUF  81      /* Buffer size for IGS record */
#define KEYPOS  72      /* Key word for extracting IGS file */

class IgesReader
{
 public:

  enum IgesEntries
  {
    nullEntry=0,
    circularArc=100,
    compositeCurve=102,
    conicArc=104,
    copiusData=106,
    plane=108,
    line=110,
    parametricSplineCurve=112,
    parametricSplineSurface=114,
    point=116,
    ruledSurface=118,
    surfaceOfRevolution=120,
    tabulatedCylinder=122,
    direction=123,
    transformationMatrix=124,
    flash=125,
    rationalBSplineCurve=126,
    rationalBSplineSurface=128,
    offsetCurve=130,
    connectPoint=132,
    node=134,
    finiteElement=136,
    nodalDisplacementAndRotation=138,
    offsetSurface=140,
    boundary=141,
    curveOnAParametricSurface=142,
    boundedSurface=143,
    trimmedSurface=144,
    nodalResults=146,
    elementResults=148,
    block=150,
    rightAngularWedge=152,
    rightCircularCylinder=154,
    rightCircularConeFrustrum=156,
    sphere=158,
    torus=160,
    solidOfRevolution=162,
    solidOfLinearExtrusion=164,
    ellipsoid=168,
    booleanTree=180,
    selectedComponent=182,
    solidAssembly=184,
    manifoldSolidB_RepObject=186,
    planeSurface=190,
    rightCircularCylindricalSurface=192,
    rightCircularConicalSurface=194,
    sphericalSurface=196,
    toroidalSurface=198,
    angularDimension=202,
    curveDimension=204,
    diameterDimension=206,
    flagNote=208,
    generalLabel=210,
    generalNote=212,
    newGeneralNote=213,
    leader=214,  //(arrow)
    linearDimension=216,
    ordinateDimension=218,
    pointDimension=220,
    radiusDimension=222,
    generalSymbol=228,
    sectionedArea=230,
    associativityDefinition=302,
    lineFontDefinition=304,
    subfigureDefinition=308,
    textFontDefinition=310,
    textDisplayTemplate=312,
    colorDefinition=314,
    unitsData=316,
    networkSubfigureDefinition=320,
    attributeTableDefinition=322,
    associativeInstance=402,
    drawing=404,
    property=406,
    singularSubfigureInstance=408,
    view=410,
    rectangularArraySubfigureInstance=412,
    circularArraySubfigureInstance=414,
    externalReference=416,
    nodalLoad_Constraint=418,
    networkSubfigureInstance=420,
    attributeTableInstance=422,
    solidInstance=430,
    vertex=502,
    edge=504,
    loop=508,
    face=510,
    discreteData=5001,
    parametricSurface=7366
  };

  double scale, tolerence;
  int units;
  FILE *fp;

  enum PositionEnum
  {
    entityPosition=0,
    sequenceNumberPosition=1,
    parameterDataPosition=2,
    matrixPosition=3,
    formPosition=3,
    visiblePosition=4,
    dependentPosition=4
  };
  
  inline int entity( int item ) const { return entityInfo(entityPosition,item); }
  inline int sequenceNumber( int item ) const { return entityInfo(sequenceNumberPosition,item); }
  inline int parameterData( int item ) const { return entityInfo(parameterDataPosition,item); }
  inline int matrix( int item ) const { return entityInfo(matrixPosition,item); }
  inline int isVisible( int item ) const { return (entityInfo(visiblePosition,item) % 2) ; }
  inline int isIndependent( int item ) const { return (entityInfo(dependentPosition,item)/2) % 2 ; }
  inline int formData( int item ) const { return entityInfo(formPosition,item); }

  long parameterPosition;  // position in file where parameter data starts

  IgesReader();
  ~IgesReader();

  int readIgesFile(const char *fileName );

  // read parameter data for a given item number
  int readData(const int & i, realArray & data, const int & numberToRead);

  // read parameter data given a pointer to the location 
  int readParameterData(const int & parameterDataPointer, realArray & data, const int & numberToRead);

  int numberOfEntities();

  int processFile();

  int getData(realArray & data, int max_data);
  
  aString entityName(const int & entity );
  
  int sequenceToItem( const int & sequence );  // return the entry in the entityInfo array for a given sequence number


 protected:

  long fieldNumber(const char *buff, int field);

  int getSequenceNumber(const char *buff);  // return seq number from Parameter line


 private:
  enum
  {
    recordBufferSize=81,  // Buffer size for Iges record
    keyWordPosition=72    // Iges key word position
  };
  

  IntegerArray entityInfo;     // holds info about objects in file (info found in the directory)

  int entityCount;

  double bound;
  char fieldDelimiter, recordDelimiter;
  int process;		/*  default process specified */
  long savep,entype,form,trans_mtr,Color,parameter,seqnum,
    formMatrix,status,visible,subordinate,
    entityUse,hierarchy,level;
  int infoLevel;		

};

#endif
