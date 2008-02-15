#ifndef TRANSFORMATION_OPTIONS_H
#define TRANSFORMATION_OPTIONS_H

class TransformationAssertion
   {
     public:
          enum TransformationOption
             {
               UnknownTransformationOption      =  0,
               LastTransformationOptionTag      = 99
             };

       // Provide a way to specify more than one option at a type (option values must be specified explicitly)
#if 1
          TransformationAssertion ( string );
          TransformationAssertion ( TransformationOption a );
          TransformationAssertion ( TransformationOption a, TransformationOption b );
          TransformationAssertion ( TransformationOption a, TransformationOption b, TransformationOption c );
          TransformationAssertion ( TransformationOption a, TransformationOption b, 
                                    TransformationOption c, TransformationOption d );
          TransformationAssertion ( TransformationOption a, TransformationOption b, 
                                    TransformationOption c, TransformationOption d, TransformationOption e );
          TransformationAssertion ( TransformationOption a, TransformationOption b, 
                                    TransformationOption c, TransformationOption d, 
                                    TransformationOption e, TransformationOption f );
#else
       // Or put them all into a single functions and use default initializers so that only a single
       // constructor need be defined
          TransformationAssertion ( TransformationOption a,
                                    TransformationOption b = UnknownTransformationOption, 
                                    TransformationOption c = UnknownTransformationOption,
                                    TransformationOption d = UnknownTransformationOption, 
                                    TransformationOption e = UnknownTransformationOption ,
                                    TransformationOption f = UnknownTransformationOption );
#endif

       // static char* getOptionString ( TransformationOption i );
          static char* getOptionString ( int i );
   };

// endif for TRANSFORMATION_OPTIONS_H
#endif























