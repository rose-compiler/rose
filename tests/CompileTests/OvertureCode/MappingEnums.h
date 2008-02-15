#ifndef MappingEnums
#define MappingEnums "MappingEnums"

  // Here are enumerators for the possible spaces for the domain and range
  enum mappingSpace
  {
    parameterSpace,    // bounds are [0,1]
    cartesianSpace     // default (-infinity,infinity)
  };

  // Here are enumerators the coordinate systems that we can use
  enum coordinateSystem
  {
    cartesian,               //  r1,r2,r3
    spherical,                 //  phi/pi, theta/2pi, r
    cylindrical,               //  theta/2pi, z, r 
    polar,                     //  r, theta/tpi, z
    toroidal                   //  theta1/tpi, theta2/tpi, theta3/tpi
  };


// Here are the enumerators for isPeriodic
  enum periodicType
  {
    notPeriodic,
    derivativePeriodic,    // Derivative is periodic but not the function
    functionPeriodic       // Function is periodic
  };
      

  // Here are enumerators for the items that we save character names for:
  enum mappingItemName
  {
    mappingClassName, // name of (derived) mapping class
    mappingName,      // mapping name
    domainName,       // domain name
    rangeName,
    domainAxis1Name, // names for coordinate axes in domain
    domainAxis2Name, 
    domainAxis3Name, 
    rangeAxis1Name,  // names for coordinate axes in range
    rangeAxis2Name, 
    rangeAxis3Name
  };

#endif // MappingEnums
