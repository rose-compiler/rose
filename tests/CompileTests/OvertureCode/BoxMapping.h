#ifndef BOX_MAPPING
#define BOX_MAPPING

#include "Mapping.h"

class BoxMapping : public Mapping{
//-------------------------------------------------------------
//  Here is a derived class to define a rectangular box in 3D 
//     Parameters:
//       (xmin,xmax) : minimum and maximum values for x(xAxis)
//       (ymin,ymax) : minimum and maximum values for x(yAxis)
//       (zmin,zmax) : minimum and maximum values for x(zAxis)
//-------------------------------------------------------------
 public:

  BoxMapping(const real xMin=0., const real xMax=1., 
             const real yMin=0., const real yMax=1.,
	     const real zMin=0., const real zMax=1.);


  // Copy constructor is deep by default
  BoxMapping( const BoxMapping &, const CopyType copyType=DEEP );

  ~BoxMapping();

  BoxMapping & operator =( const BoxMapping & X );

  int rotate(const real angle, 
             const int axisOfRotation = 2 ,
	     const real x0 = 0.,
	     const real y0 = 0.,
	     const real z0 = 0. );
  
  int setVertices(const real & xMin=0., const real & xMax=1., 
		  const real & yMin=0., const real & yMax=1.,
		  const real & zMin=0., const real & zMax=1.);

  int getVertices(real & xMin, real & xMax, real & yMin, real & yMax, real & zMin, real & zMax ) const; 

  virtual int get( const GenericDataBase & dir, const aString & name);    // get from a database file
  virtual int put( GenericDataBase & dir, const aString & name) const;    // put to a database file

  aString getClassName() const { return BoxMapping::className; }

  Mapping* make( const aString & mappingClassName );

  void map( const realArray & r, realArray & x, realArray & xr = Overture::nullRealDistributedArray(),
            MappingParameters & params=Overture::nullMappingParameters());

  void basicInverse( const realArray & x, realArray & r, realArray & rx= Overture::nullRealDistributedArray(), 
                     MappingParameters & params =Overture::nullMappingParameters());

  int update( MappingInformation & mapInfo ) ;

 protected:
  aString className;
  real xa[3],xb[3];
  int rotationAxis;
  real rotationAngle, centerOfRotation[3];

  private:

  //
  //  Virtual member functions used only through class ReferenceCounting:
  //
    virtual ReferenceCounting& operator=(const ReferenceCounting& x)
      { return operator=((BoxMapping &)x); }
    virtual void reference( const ReferenceCounting& x) 
      { reference((BoxMapping &)x); }     // *** Conversion to this class for the virtual = ****
    virtual ReferenceCounting* virtualConstructor( const CopyType ct = DEEP ) const
      { return ::new BoxMapping(*this, ct); }
};

#endif
