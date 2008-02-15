#ifndef TrivialArray_
#define TrivialArray_

#include <assert.h>

// template<class Type, class Dim>
class TrivialArray {
  public:
// //
// //  Change this section to allow for any maximum number of dimensions:
// //  Also add more array indexing functions using operator().
// //
// //  Allow up to ten-dimensional arrays.
//     enum { MAXDIM = 10 };
// #define DIM_ARGUMENTS                                 \
//       const Dim& dim0 = 0, const Dim& dim1 = 0,       \
//       const Dim& dim2 = 0, const Dim& dim3 = 0,       \
//       const Dim& dim4 = 0, const Dim& dim5 = 0,       \
//       const Dim& dim6 = 0, const Dim& dim7 = 0,       \
//       const Dim& dim8 = 0, const Dim& dim9 = 0
// #define DIM_ARGS dim0, dim1, dim2, dim3, dim4, dim5, dim6, dim7, dim8, dim9
// #define X_DIM_ARGS                                    \
//       x.dimension[0], x.dimension[1], x.dimension[2], \
//       x.dimension[3], x.dimension[4], x.dimension[5], \
//       x.dimension[6], x.dimension[7], x.dimension[8], \
//       x.dimension[9]
// #define DIM_DECLARATIONS                              \
//     Dim dim[MAXDIM];                                  \
//     dim[0] = dim0; dim[1] = dim1; dim[2] = dim2;      \
//     dim[3] = dim3; dim[4] = dim4; dim[5] = dim5;      \
//     dim[6] = dim6; dim[7] = dim7; dim[8] = dim8;      \
//     dim[9] = dim9
// //
// //  Public member functions:
// //
// //  Default constructor, with optional arguments of type Dim.
// //
// #ifdef USE_DEFAULT_ARGUMENTS
//     TrivialArray(DIM_ARGUMENTS) {
//         numberOfElements = numberOfDimensions = 0;
//         dataPointer = basePointer = 0;
//         for (int i=0; i<MAXDIM; i++)
//           { dimension[i] = Dim(0); stride[i] = 1; }
//         redim(DIM_ARGS);
//     }
// #else
// #define TrivialArray_constructor                      \
//         numberOfElements = numberOfDimensions = 0;    \
//         dataPointer = basePointer = 0;                \
//         for (int i=0; i<MAXDIM; i++)                  \
//           { dimension[i] = Dim(0); stride[i] = 1; }

//     TrivialArray() {
//         TrivialArray_constructor;
//         redim();
//     }
//     TrivialArray(const Dim& dim0) {
//         TrivialArray_constructor;
//         redim(dim0);
//     }
//     TrivialArray(const Dim& dim0, const Dim& dim1) {
//         TrivialArray_constructor;
//         redim(dim0, dim1);
//     }
//     TrivialArray(const Dim& dim0, const Dim& dim1, const Dim& dim2) {
//         TrivialArray_constructor;
//         redim(dim0, dim1, dim2);
//     }
//     TrivialArray(const Dim& dim0, const Dim& dim1, const Dim& dim2,
//       const Dim& dim3) {
//         TrivialArray_constructor;
//         redim(dim0, dim1, dim2, dim3);
//     }
//     TrivialArray(const Dim& dim0, const Dim& dim1, const Dim& dim2,
//       const Dim& dim3, const Dim& dim4) {
//         TrivialArray_constructor;
//         redim(dim0, dim1, dim2, dim3, dim4);
//     }
//     TrivialArray(const Dim& dim0, const Dim& dim1, const Dim& dim2,
//       const Dim& dim3, const Dim& dim4, const Dim& dim5) {
//         TrivialArray_constructor;
//         redim(dim0, dim1, dim2, dim3, dim4, dim5);
//     }
//     TrivialArray(const Dim& dim0, const Dim& dim1, const Dim& dim2,
//       const Dim& dim3, const Dim& dim4, const Dim& dim5, const Dim& dim6) {
//         TrivialArray_constructor;
//         redim(dim0, dim1, dim2, dim3, dim4, dim5, dim6);
//     }
//     TrivialArray(const Dim& dim0, const Dim& dim1, const Dim& dim2,
//       const Dim& dim3, const Dim& dim4, const Dim& dim5, const Dim& dim6,
//       const Dim& dim7) {
//         TrivialArray_constructor;
//         redim(dim0, dim1, dim2, dim3, dim4, dim5, dim6, dim7);
//     }
//     TrivialArray(const Dim& dim0, const Dim& dim1, const Dim& dim2,
//       const Dim& dim3, const Dim& dim4, const Dim& dim5, const Dim& dim6,
//       const Dim& dim7, const Dim& dim8) {
//         TrivialArray_constructor;
//         redim(dim0, dim1, dim2, dim3, dim4, dim5, dim6, dim7, dim8);
//     }
//     TrivialArray(const Dim& dim0, const Dim& dim1, const Dim& dim2,
//       const Dim& dim3, const Dim& dim4, const Dim& dim5, const Dim& dim6,
//       const Dim& dim7, const Dim& dim8, const Dim& dim9) {
//         TrivialArray_constructor;
//         redim(dim0, dim1, dim2, dim3, dim4, dim5, dim6, dim7, dim8, dim9);
//     }
// #undef TrivialArray_constructor
// #endif // USE_DEFAULT_ARGUMENTS
// //
// //  Copy constructor.
// //
//     TrivialArray(const TrivialArray<Type,Dim>& x) {
//         numberOfElements = numberOfDimensions = 0;
//         dataPointer = basePointer = 0;
//         for (int i=0; i<MAXDIM; i++)
//           { dimension[i] = Dim(0); stride[i] = 1; }
//         operator=(x);
//     }
// //
// //  Destructor.
// //
//     ~TrivialArray()
//       { if (dataPointer) delete [] dataPointer; }
// //
// //  Reshape.  Arguments of type Dim are optional.
// //
// #ifdef USE_DEFAULT_ARGUMENTS
//     int reshape(DIM_ARGUMENTS) {
//         DIM_DECLARATIONS;
//         int numberOfElements_, numberOfDimensions_, dimsChanged, i;
//         for (numberOfElements_ = numberOfDimensions_ = dimsChanged = 0;
//           numberOfDimensions_ < MAXDIM && dim[numberOfDimensions_] != Dim(0);
//           numberOfDimensions_++)
//           if (numberOfDimensions_)
//                numberOfElements_ *= dim[numberOfDimensions_].getCount();
//           else numberOfElements_  = dim[numberOfDimensions_].getCount();
//         assert(numberOfElements_ == numberOfElements);
//         dimsChanged = numberOfDimensions_ != numberOfDimensions;
//         numberOfDimensions = numberOfDimensions_;
//         basePointer = dataPointer;
//         for (i=0; i<numberOfDimensions; i++) {
//             dimsChanged = dimsChanged || dim[i] != dimension[i];
//             dimension[i] = dim[i];
//             stride[i] = i ? stride[i-1] * dimension[i-1].getCount() : 1;
//             basePointer -= stride[i] * dimension[i].getBase();
//         } // end for
//         for (i=numberOfDimensions; i<MAXDIM; i++) {
//             dimension[i] = Dim(0);
//             stride[i] = i ? stride[i-1] : 1;
//         } // end for
//         return dimsChanged;
//     }
// #else
// #define TrivialArray_reshape                                                   \
//     int numberOfElements_, numberOfDimensions_, dimsChanged, i;                \
//     for (numberOfElements_ = numberOfDimensions_ = dimsChanged = 0;            \
//       numberOfDimensions_ < MAXDIM && !(dim[numberOfDimensions_] == Dim(0));   \
//       numberOfDimensions_++)                                                   \
//       if (numberOfDimensions_)                                                 \
//            numberOfElements_ *= dim[numberOfDimensions_].getCount();           \
//       else numberOfElements_  = dim[numberOfDimensions_].getCount();           \
//     assert(numberOfElements_ == numberOfElements);                             \
//     dimsChanged = numberOfDimensions_ != numberOfDimensions;                   \
//     numberOfDimensions = numberOfDimensions_;                                  \
//     basePointer = dataPointer;                                                 \
//     for (i=0; i<numberOfDimensions; i++) {                                     \
//         dimsChanged = dimsChanged || !(dim[i] == dimension[i]);                \
//         dimension[i] = dim[i];                                                 \
//         stride[i] = i ? stride[i-1] * dimension[i-1].getCount() : 1;           \
//         basePointer -= stride[i] * dimension[i].getBase();                     \
//     }                                                                          \
//     for (i=numberOfDimensions; i<MAXDIM; i++) {                                \
//         dimension[i] = Dim(0);                                                 \
//         stride[i] = i ? stride[i-1] : 1;                                       \
//     }                                                                          \
//     return dimsChanged
//     int reshape() {
//         Dim dim[MAXDIM];
//         dim[0] = 0; dim[1] = 0; dim[2] = 0; dim[3] = 0; dim[4] = 0;
//         dim[5] = 0; dim[6] = 0; dim[7] = 0; dim[8] = 0; dim[9] = 0;
//         TrivialArray_reshape;
//     }
//     int reshape(const Dim& dim0) {
//         Dim dim[MAXDIM];
//         dim[0] = dim0; dim[1] = 0; dim[2] = 0; dim[3] = 0; dim[4] = 0;
//         dim[5] = 0;    dim[6] = 0; dim[7] = 0; dim[8] = 0; dim[9] = 0;
//         TrivialArray_reshape;
//     }
//     int reshape(const Dim& dim0, const Dim& dim1) {
//         Dim dim[MAXDIM];
//         dim[0] = dim0; dim[1] = dim1; dim[2] = 0; dim[3] = 0; dim[4] = 0;
//         dim[5] = 0;    dim[6] = 0;    dim[7] = 0; dim[8] = 0; dim[9] = 0;
//         TrivialArray_reshape;
//     }
//     int reshape(const Dim& dim0, const Dim& dim1, const Dim& dim2) {
//         Dim dim[MAXDIM];
//         dim[0] = dim0; dim[1] = dim1; dim[2] = dim2; dim[3] = 0; dim[4] = 0;
//         dim[5] = 0;    dim[6] = 0;    dim[7] = 0;    dim[8] = 0; dim[9] = 0;
//         TrivialArray_reshape;
//     }
//     int reshape(const Dim& dim0, const Dim& dim1, const Dim& dim2,
//       const Dim& dim3) {
//         Dim dim[MAXDIM];
//         dim[0] = dim0; dim[1] = dim1; dim[2] = dim2; dim[3] = dim3; dim[4] = 0;
//         dim[5] = 0;    dim[6] = 0;    dim[7] = 0;    dim[8] = 0;    dim[9] = 0;
//         TrivialArray_reshape;
//     }
//     int reshape(const Dim& dim0, const Dim& dim1, const Dim& dim2,
//       const Dim& dim3, const Dim& dim4) {
//         Dim dim[MAXDIM];
//         dim[0] = dim0; dim[1] = dim1; dim[2] = dim2; dim[3] = dim3;
//         dim[4] = dim4; dim[5] = 0;    dim[6] = 0;    dim[7] = 0;
//         dim[8] = 0;    dim[9] = 0;
//         TrivialArray_reshape;
//     }
//     int reshape(const Dim& dim0, const Dim& dim1, const Dim& dim2,
//       const Dim& dim3, const Dim& dim4, const Dim& dim5) {
//         Dim dim[MAXDIM];
//         dim[0] = dim0; dim[1] = dim1; dim[2] = dim2; dim[3] = dim3;
//         dim[4] = dim4; dim[5] = dim5; dim[6] = 0;    dim[7] = 0;
//         dim[8] = 0;    dim[9] = 0;
//         TrivialArray_reshape;
//     }
//     int reshape(const Dim& dim0, const Dim& dim1, const Dim& dim2,
//       const Dim& dim3, const Dim& dim4, const Dim& dim5, const Dim& dim6) {
//         Dim dim[MAXDIM];
//         dim[0] = dim0; dim[1] = dim1; dim[2] = dim2; dim[3] = dim3;
//         dim[4] = dim4; dim[5] = dim5; dim[6] = dim6; dim[7] = 0;
//         dim[8] = 0;    dim[9] = 0;
//         TrivialArray_reshape;
//     }
//     int reshape(const Dim& dim0, const Dim& dim1, const Dim& dim2,
//       const Dim& dim3, const Dim& dim4, const Dim& dim5, const Dim& dim6,
//       const Dim& dim7) {
//         Dim dim[MAXDIM];
//         dim[0] = dim0; dim[1] = dim1; dim[2] = dim2; dim[3] = dim3;
//         dim[4] = dim4; dim[5] = dim5; dim[6] = dim6; dim[7] = dim7;
//         dim[8] = 0;    dim[9] = 0;
//         TrivialArray_reshape;
//     }
//     int reshape(const Dim& dim0, const Dim& dim1, const Dim& dim2,
//       const Dim& dim3, const Dim& dim4, const Dim& dim5, const Dim& dim6,
//       const Dim& dim7, const Dim& dim8) {
//         Dim dim[MAXDIM];
//         dim[0] = dim0; dim[1] = dim1; dim[2] = dim2; dim[3] = dim3;
//         dim[4] = dim4; dim[5] = dim5; dim[6] = dim6; dim[7] = dim7;
//         dim[8] = dim8; dim[9] = 0;
//         TrivialArray_reshape;
//     }
//     int reshape(const Dim& dim0, const Dim& dim1, const Dim& dim2,
//       const Dim& dim3, const Dim& dim4, const Dim& dim5, const Dim& dim6,
//       const Dim& dim7, const Dim& dim8, const Dim& dim9) {
//         Dim dim[MAXDIM];
//         dim[0] = dim0; dim[1] = dim1; dim[2] = dim2; dim[3] = dim3;
//         dim[4] = dim4; dim[5] = dim5; dim[6] = dim6; dim[7] = dim7;
//         dim[8] = dim8; dim[9] = dim9;
//         TrivialArray_reshape;
//     }
// #undef TrivialArray_reshape
// #endif // USE_DEFAULT_ARGUMENTS
//     int reshape(const TrivialArray& x) { return reshape(X_DIM_ARGS); }
// //
// //  Redimension (does not copy data).  Arguments of type Dim are optional.
// //
// #ifdef USE_DEFAULT_ARGUMENTS
//     int redim(DIM_ARGUMENTS) {
//         DIM_DECLARATIONS;
//         int numberOfElements_, numberOfDimensions_, dimsChanged, i;
//         for (numberOfElements_ = numberOfDimensions_ = dimsChanged = 0;
//           numberOfDimensions_ < MAXDIM && dim[numberOfDimensions_] != Dim(0);
//           numberOfDimensions_++) {
//             if (numberOfDimensions_)
//                  numberOfElements_ *= dim[numberOfDimensions_].getCount();
//             else numberOfElements_  = dim[numberOfDimensions_].getCount();
//             dimsChanged = dimsChanged ||
//               numberOfDimensions_ >= numberOfDimensions ||
//               dim[numberOfDimensions_] != dimension[numberOfDimensions_];
//         } // end for
//         dimsChanged = dimsChanged || numberOfDimensions_ != numberOfDimensions;
//         if (numberOfElements != numberOfElements_) {
//             assert(dimsChanged);
//             if (dataPointer) delete [] dataPointer;
//             dataPointer = (numberOfElements = numberOfElements_) ?
//               new Type[numberOfElements] : 0;
//         } // end if
//         if (dimsChanged) {
//             numberOfDimensions = numberOfDimensions_;
//             basePointer = dataPointer;
//             for (i=0; i<numberOfDimensions; i++) {
//                 dimension[i] = dim[i];
//                 stride[i] = i ? stride[i-1] * dimension[i-1].getCount() : 1;
//                 basePointer -= stride[i] * dimension[i].getBase();
//             } // end for
//             for (i=numberOfDimensions; i<MAXDIM; i++)
//               { dimension[i] = Dim(0); stride[i] = i ? stride[i-1] : 1; }
//         } // end if
//         return dimsChanged;
//     }
// #else
// #define TrivialArray_redim                                                     \
//     int numberOfElements_, numberOfDimensions_, dimsChanged, i;                \
//     for (numberOfElements_ = numberOfDimensions_ = dimsChanged = 0;            \
//       numberOfDimensions_ < MAXDIM && !(dim[numberOfDimensions_] == Dim(0));   \
//       numberOfDimensions_++) {                                                 \
//         if (numberOfDimensions_)                                               \
//              numberOfElements_ *= dim[numberOfDimensions_].getCount();         \
//         else numberOfElements_  = dim[numberOfDimensions_].getCount();         \
//         dimsChanged = dimsChanged ||                                           \
//           numberOfDimensions_ >= numberOfDimensions ||                         \
//           !(dim[numberOfDimensions_] == dimension[numberOfDimensions_]);       \
//     }                                                                          \
//     dimsChanged = dimsChanged || numberOfDimensions_ != numberOfDimensions;    \
//     if (numberOfElements != numberOfElements_) {                               \
//         assert(dimsChanged);                                                   \
//         if (dataPointer) delete [] dataPointer;                                \
//         dataPointer = (numberOfElements = numberOfElements_) ?                 \
//           new Type[numberOfElements] : 0;                                      \
//     }                                                                          \
//     if (dimsChanged) {                                                         \
//         numberOfDimensions = numberOfDimensions_;                              \
//         basePointer = dataPointer;                                             \
//         for (i=0; i<numberOfDimensions; i++) {                                 \
//             dimension[i] = dim[i];                                             \
//             stride[i] = i ? stride[i-1] * dimension[i-1].getCount() : 1;       \
//             basePointer -= stride[i] * dimension[i].getBase();                 \
//         }                                                                      \
//         for (i=numberOfDimensions; i<MAXDIM; i++)                              \
//           { dimension[i] = Dim(0); stride[i] = i ? stride[i-1] : 1; }          \
//     }                                                                          \
//     return dimsChanged
//     int redim() {
//         Dim dim[MAXDIM];
//         dim[0] = 0; dim[1] = 0; dim[2] = 0; dim[3] = 0; dim[4] = 0;
//         dim[5] = 0; dim[6] = 0; dim[7] = 0; dim[8] = 0; dim[9] = 0;
//         TrivialArray_redim;
//     }
//     int redim(const Dim& dim0) {
//         Dim dim[MAXDIM];
//         dim[0] = dim0; dim[1] = 0; dim[2] = 0; dim[3] = 0; dim[4] = 0;
//         dim[5] = 0;    dim[6] = 0; dim[7] = 0; dim[8] = 0; dim[9] = 0;
//         TrivialArray_redim;
//     }
//     int redim(const Dim& dim0, const Dim& dim1) {
//         Dim dim[MAXDIM];
//         dim[0] = dim0; dim[1] = dim1; dim[2] = 0; dim[3] = 0; dim[4] = 0;
//         dim[5] = 0;    dim[6] = 0;    dim[7] = 0; dim[8] = 0; dim[9] = 0;
//         TrivialArray_redim;
//     }
//     int redim(const Dim& dim0, const Dim& dim1, const Dim& dim2) {
//         Dim dim[MAXDIM];
//         dim[0] = dim0; dim[1] = dim1; dim[2] = dim2; dim[3] = 0; dim[4] = 0;
//         dim[5] = 0;    dim[6] = 0;    dim[7] = 0;    dim[8] = 0; dim[9] = 0;
//         TrivialArray_redim;
//     }
//     int redim(const Dim& dim0, const Dim& dim1, const Dim& dim2,
//       const Dim& dim3) {
//         Dim dim[MAXDIM];
//         dim[0] = dim0; dim[1] = dim1; dim[2] = dim2; dim[3] = dim3; dim[4] = 0;
//         dim[5] = 0;    dim[6] = 0;    dim[7] = 0;    dim[8] = 0;    dim[9] = 0;
//         TrivialArray_redim;
//     }
//     int redim(const Dim& dim0, const Dim& dim1, const Dim& dim2,
//       const Dim& dim3, const Dim& dim4) {
//         Dim dim[MAXDIM];
//         dim[0] = dim0; dim[1] = dim1; dim[2] = dim2; dim[3] = dim3;
//         dim[4] = dim4; dim[5] = 0;    dim[6] = 0;    dim[7] = 0;
//         dim[8] = 0;    dim[9] = 0;
//         TrivialArray_redim;
//     }
//     int redim(const Dim& dim0, const Dim& dim1, const Dim& dim2,
//       const Dim& dim3, const Dim& dim4, const Dim& dim5) {
//         Dim dim[MAXDIM];
//         dim[0] = dim0; dim[1] = dim1; dim[2] = dim2; dim[3] = dim3;
//         dim[4] = dim4; dim[5] = dim5; dim[6] = 0;    dim[7] = 0;
//         dim[8] = 0;    dim[9] = 0;
//         TrivialArray_redim;
//     }
//     int redim(const Dim& dim0, const Dim& dim1, const Dim& dim2,
//       const Dim& dim3, const Dim& dim4, const Dim& dim5, const Dim& dim6) {
//         Dim dim[MAXDIM];
//         dim[0] = dim0; dim[1] = dim1; dim[2] = dim2; dim[3] = dim3;
//         dim[4] = dim4; dim[5] = dim5; dim[6] = dim6; dim[7] = 0;
//         dim[8] = 0;    dim[9] = 0;
//         TrivialArray_redim;
//     }
//     int redim(const Dim& dim0, const Dim& dim1, const Dim& dim2,
//       const Dim& dim3, const Dim& dim4, const Dim& dim5, const Dim& dim6,
//       const Dim& dim7) {
//         Dim dim[MAXDIM];
//         dim[0] = dim0; dim[1] = dim1; dim[2] = dim2; dim[3] = dim3;
//         dim[4] = dim4; dim[5] = dim5; dim[6] = dim6; dim[7] = dim7;
//         dim[8] = 0;    dim[9] = 0;
//         TrivialArray_redim;
//     }
//     int redim(const Dim& dim0, const Dim& dim1, const Dim& dim2,
//       const Dim& dim3, const Dim& dim4, const Dim& dim5, const Dim& dim6,
//       const Dim& dim7, const Dim& dim8) {
//         Dim dim[MAXDIM];
//         dim[0] = dim0; dim[1] = dim1; dim[2] = dim2; dim[3] = dim3;
//         dim[4] = dim4; dim[5] = dim5; dim[6] = dim6; dim[7] = dim7;
//         dim[8] = dim8; dim[9] = 0;
//         TrivialArray_redim;
//     }
//     int redim(const Dim& dim0, const Dim& dim1, const Dim& dim2,
//       const Dim& dim3, const Dim& dim4, const Dim& dim5, const Dim& dim6,
//       const Dim& dim7, const Dim& dim8, const Dim& dim9) {
//         Dim dim[MAXDIM];
//         dim[0] = dim0; dim[1] = dim1; dim[2] = dim2; dim[3] = dim3;
//         dim[4] = dim4; dim[5] = dim5; dim[6] = dim6; dim[7] = dim7;
//         dim[8] = dim8; dim[9] = dim9;
//         TrivialArray_redim;
//     }
// #undef TrivialArray_redim
// #endif // USE_DEFAULT_ARGUMENTS
//     int redim(const TrivialArray& x) { return redim(X_DIM_ARGS); }
// //
// //  Resize (Copies data).  Arguments of type Dim are optional.
// //
// #ifdef USE_DEFAULT_ARGUMENTS
//     int resize(DIM_ARGUMENTS) {
//         DIM_DECLARATIONS;
//         Dim dimension_[MAXDIM];
//         int stride_[MAXDIM], base[MAXDIM], bound[MAXDIM];
//         int numberOfElements_, numberOfDimensions_, dimsChanged,
//           dimsOverlap = 1, i;
//         for (numberOfElements_ = numberOfDimensions_ = dimsChanged = 0;
//           numberOfDimensions_ < MAXDIM && dim[numberOfDimensions_] != Dim(0);
//           numberOfDimensions_++) {
//             dimension_[numberOfDimensions_] = dimension[numberOfDimensions_];
//             if ((base[numberOfDimensions_] = max0(
//                    dimension[numberOfDimensions_].getBase(),
//                    dim[numberOfDimensions_]      .getBase())) >
//                 (bound[numberOfDimensions_] = min0(
//                    dimension[numberOfDimensions_].getBound(),
//                    dim[numberOfDimensions_]      .getBound())))
//               dimsOverlap = 0;
//             stride_[numberOfDimensions_] = stride[numberOfDimensions_];
//             if (numberOfDimensions_)
//                  numberOfElements_ *= dim[numberOfDimensions_].getCount();
//             else numberOfElements_  = dim[numberOfDimensions_].getCount();
//             dimsChanged = dimsChanged ||
//               numberOfDimensions_ >= numberOfDimensions ||
//               dim[numberOfDimensions_] != dimension[numberOfDimensions_];
//         } // end for
//         assert (numberOfDimensions_ == numberOfDimensions);
//         if (dimsChanged) {
//             Type *dataPointer_ = dataPointer, *basePointer_ = basePointer;
//             dataPointer = (numberOfElements = numberOfElements_) ?
//               new Type[numberOfElements] : 0;
//             basePointer = dataPointer;
//             for (i=0; i<numberOfDimensions; i++) {
//                 dimension[i] = dim[i];
//                 stride[i] = i ? stride[i-1] * dimension[i-1].getCount() : 1;
//                 basePointer -= stride[i] * dimension[i].getBase();
//             } // end for
//             for (i=numberOfDimensions; i<MAXDIM; i++) {
//                 dimension[i] = Dim(0); stride[i] = i ? stride[i-1] : 1;
//                 base[i] = bound[i] = dimension[i].getBase();
//             } // end for 
//             if (dimsOverlap)
//               for (int i9=base[9]; i9<=bound[9]; i9++)
//               for (int i8=base[8]; i8<=bound[8]; i8++)
//               for (int i7=base[7]; i7<=bound[7]; i7++)
//               for (int i6=base[6]; i6<=bound[6]; i6++)
//               for (int i5=base[5]; i5<=bound[5]; i5++)
//               for (int i4=base[4]; i4<=bound[4]; i4++)
//               for (int i3=base[3]; i3<=bound[3]; i3++)
//               for (int i2=base[2]; i2<=bound[2]; i2++)
//               for (int i1=base[1]; i1<=bound[1]; i1++)
//               for (int i0=base[0]; i0<=bound[0]; i0++)
//                 basePointer[  i0 +
//                   stride[1] * i1 + stride[2] * i2 + stride[3] * i3 +
//                   stride[4] * i4 + stride[5] * i5 + stride[6] * i6 +
//                   stride[7] * i7 + stride[8] * i8 + stride[9] * i9] =
//                   basePointer_[  i0 +
//                     stride_[1] * i1 + stride_[2] * i2 + stride_[3] * i3 +
//                     stride_[4] * i4 + stride_[5] * i5 + stride_[6] * i6 +
//                     stride_[7] * i7 + stride_[8] * i8 + stride_[9] * i9];
//             if (dataPointer_) delete [] dataPointer_;
//         } // end if
//         return dimsChanged;
//     }
// #else
// #define TrivialArray_resize                                                    \
//     Dim dimension_[MAXDIM];                                                    \
//     int stride_[MAXDIM], base[MAXDIM], bound[MAXDIM];                          \
//     int numberOfElements_, numberOfDimensions_, dimsChanged,                   \
//       dimsOverlap = 1, i;                                                      \
//     for (numberOfElements_ = numberOfDimensions_ = dimsChanged = 0;            \
//       numberOfDimensions_ < MAXDIM && !(dim[numberOfDimensions_] == Dim(0));   \
//       numberOfDimensions_++) {                                                 \
//         dimension_[numberOfDimensions_] = dimension[numberOfDimensions_];      \
//         if ((base[numberOfDimensions_] = max0(                                 \
//                dimension[numberOfDimensions_].getBase(),                       \
//                dim[numberOfDimensions_]      .getBase())) >                    \
//             (bound[numberOfDimensions_] = min0(                                \
//                dimension[numberOfDimensions_].getBound(),                      \
//                dim[numberOfDimensions_]      .getBound())))                    \
//           dimsOverlap = 0;                                                     \
//         stride_[numberOfDimensions_] = stride[numberOfDimensions_];            \
//         if (numberOfDimensions_)                                               \
//              numberOfElements_ *= dim[numberOfDimensions_].getCount();         \
//         else numberOfElements_  = dim[numberOfDimensions_].getCount();         \
//         dimsChanged = dimsChanged ||                                           \
//           numberOfDimensions_ >= numberOfDimensions ||                         \
//           !(dim[numberOfDimensions_] == dimension[numberOfDimensions_]);       \
//     }                                                                          \
//     assert (numberOfDimensions_ == numberOfDimensions);                        \
//     if (dimsChanged) {                                                         \
//         Type *dataPointer_ = dataPointer, *basePointer_ = basePointer;         \
//         dataPointer = (numberOfElements = numberOfElements_) ?                 \
//           new Type[numberOfElements] : 0;                                      \
//         basePointer = dataPointer;                                             \
//         for (i=0; i<numberOfDimensions; i++) {                                 \
//             dimension[i] = dim[i];                                             \
//             stride[i] = i ? stride[i-1] * dimension[i-1].getCount() : 1;       \
//             basePointer -= stride[i] * dimension[i].getBase();                 \
//         }                                                                      \
//         for (i=numberOfDimensions; i<MAXDIM; i++) {                            \
//             dimension[i] = Dim(0); stride[i] = i ? stride[i-1] : 1;            \
//             base[i] = bound[i] = dimension[i].getBase();                       \
//         }                                                                      \
//         if (dimsOverlap)                                                       \
//           for (int i9=base[9]; i9<=bound[9]; i9++)                             \
//           for (int i8=base[8]; i8<=bound[8]; i8++)                             \
//           for (int i7=base[7]; i7<=bound[7]; i7++)                             \
//           for (int i6=base[6]; i6<=bound[6]; i6++)                             \
//           for (int i5=base[5]; i5<=bound[5]; i5++)                             \
//           for (int i4=base[4]; i4<=bound[4]; i4++)                             \
//           for (int i3=base[3]; i3<=bound[3]; i3++)                             \
//           for (int i2=base[2]; i2<=bound[2]; i2++)                             \
//           for (int i1=base[1]; i1<=bound[1]; i1++)                             \
//           for (int i0=base[0]; i0<=bound[0]; i0++)                             \
//             basePointer[  i0 +                                                 \
//               stride[1] * i1 + stride[2] * i2 + stride[3] * i3 +               \
//               stride[4] * i4 + stride[5] * i5 + stride[6] * i6 +               \
//               stride[7] * i7 + stride[8] * i8 + stride[9] * i9] =              \
//               basePointer_[  i0 +                                              \
//                 stride_[1] * i1 + stride_[2] * i2 + stride_[3] * i3 +          \
//                 stride_[4] * i4 + stride_[5] * i5 + stride_[6] * i6 +          \
//                 stride_[7] * i7 + stride_[8] * i8 + stride_[9] * i9];          \
//         if (dataPointer_) delete [] dataPointer_;                              \
//     }                                                                          \
//     return dimsChanged
//     int resize() {
//         Dim dim[MAXDIM];
//         dim[0] = 0; dim[1] = 0; dim[2] = 0; dim[3] = 0; dim[4] = 0;
//         dim[5] = 0; dim[6] = 0; dim[7] = 0; dim[8] = 0; dim[9] = 0;
//         TrivialArray_resize;
//     }
//     int resize(const Dim& dim0) {
//         Dim dim[MAXDIM];
//         dim[0] = dim0; dim[1] = 0; dim[2] = 0; dim[3] = 0; dim[4] = 0;
//         dim[5] = 0;    dim[6] = 0; dim[7] = 0; dim[8] = 0; dim[9] = 0;
//         TrivialArray_resize;
//     }
//     int resize(const Dim& dim0, const Dim& dim1) {
//         Dim dim[MAXDIM];
//         dim[0] = dim0; dim[1] = dim1; dim[2] = 0; dim[3] = 0; dim[4] = 0;
//         dim[5] = 0;    dim[6] = 0;    dim[7] = 0; dim[8] = 0; dim[9] = 0;
//         TrivialArray_resize;
//     }
//     int resize(const Dim& dim0, const Dim& dim1, const Dim& dim2) {
//         Dim dim[MAXDIM];
//         dim[0] = dim0; dim[1] = dim1; dim[2] = dim2; dim[3] = 0; dim[4] = 0;
//         dim[5] = 0;    dim[6] = 0;    dim[7] = 0;    dim[8] = 0; dim[9] = 0;
//         TrivialArray_resize;
//     }
//     int resize(const Dim& dim0, const Dim& dim1, const Dim& dim2,
//       const Dim& dim3) {
//         Dim dim[MAXDIM];
//         dim[0] = dim0; dim[1] = dim1; dim[2] = dim2; dim[3] = dim3; dim[4] = 0;
//         dim[5] = 0;    dim[6] = 0;    dim[7] = 0;    dim[8] = 0;    dim[9] = 0;
//         TrivialArray_resize;
//     }
//     int resize(const Dim& dim0, const Dim& dim1, const Dim& dim2,
//       const Dim& dim3, const Dim& dim4) {
//         Dim dim[MAXDIM];
//         dim[0] = dim0; dim[1] = dim1; dim[2] = dim2; dim[3] = dim3;
//         dim[4] = dim4; dim[5] = 0;    dim[6] = 0;    dim[7] = 0;
//         dim[8] = 0;    dim[9] = 0;
//         TrivialArray_resize;
//     }
//     int resize(const Dim& dim0, const Dim& dim1, const Dim& dim2,
//       const Dim& dim3, const Dim& dim4, const Dim& dim5) {
//         Dim dim[MAXDIM];
//         dim[0] = dim0; dim[1] = dim1; dim[2] = dim2; dim[3] = dim3;
//         dim[4] = dim4; dim[5] = dim5; dim[6] = 0;    dim[7] = 0;
//         dim[8] = 0;    dim[9] = 0;
//         TrivialArray_resize;
//     }
//     int resize(const Dim& dim0, const Dim& dim1, const Dim& dim2,
//       const Dim& dim3, const Dim& dim4, const Dim& dim5, const Dim& dim6) {
//         Dim dim[MAXDIM];
//         dim[0] = dim0; dim[1] = dim1; dim[2] = dim2; dim[3] = dim3;
//         dim[4] = dim4; dim[5] = dim5; dim[6] = dim6; dim[7] = 0;
//         dim[8] = 0;    dim[9] = 0;
//         TrivialArray_resize;
//     }
//     int resize(const Dim& dim0, const Dim& dim1, const Dim& dim2,
//       const Dim& dim3, const Dim& dim4, const Dim& dim5, const Dim& dim6,
//       const Dim& dim7) {
//         Dim dim[MAXDIM];
//         dim[0] = dim0; dim[1] = dim1; dim[2] = dim2; dim[3] = dim3;
//         dim[4] = dim4; dim[5] = dim5; dim[6] = dim6; dim[7] = dim7;
//         dim[8] = 0;    dim[9] = 0;
//         TrivialArray_resize;
//     }
//     int resize(const Dim& dim0, const Dim& dim1, const Dim& dim2,
//       const Dim& dim3, const Dim& dim4, const Dim& dim5, const Dim& dim6,
//       const Dim& dim7, const Dim& dim8) {
//         Dim dim[MAXDIM];
//         dim[0] = dim0; dim[1] = dim1; dim[2] = dim2; dim[3] = dim3;
//         dim[4] = dim4; dim[5] = dim5; dim[6] = dim6; dim[7] = dim7;
//         dim[8] = dim8; dim[9] = 0;
//         TrivialArray_resize;
//     }
//     int resize(const Dim& dim0, const Dim& dim1, const Dim& dim2,
//       const Dim& dim3, const Dim& dim4, const Dim& dim5, const Dim& dim6,
//       const Dim& dim7, const Dim& dim8, const Dim& dim9) {
//         Dim dim[MAXDIM];
//         dim[0] = dim0; dim[1] = dim1; dim[2] = dim2; dim[3] = dim3;
//         dim[4] = dim4; dim[5] = dim5; dim[6] = dim6; dim[7] = dim7;
//         dim[8] = dim8; dim[9] = dim9;
//         TrivialArray_resize;
//     }
// #undef TrivialArray_resize
// #endif // USE_DEFAULT_ARGUMENTS
//     int resize(const TrivialArray& x) { return resize(X_DIM_ARGS); }
// //
// //  Operator =.
// //
//     TrivialArray& operator=(const TrivialArray<Type,Dim>& x) {
//         if (this != &x) {
//             if (numberOfElements == 0) {
//                 redim(x);
//             } else {
//                 assert(numberOfElements   == x.numberOfElements);
//                 assert(numberOfDimensions == x.numberOfDimensions);
//                 for (int i=0; i<MAXDIM; i++)
//                   assert(dimension[i] == x.dimension[i]);
//             } // end if
//             for (int i=0; i<numberOfElements; i++)
//               dataPointer[i] = x.dataPointer[i];
//         } // end if
//         return *this;
//     }
//     TrivialArray& operator=(const Type& x) {
//         for (int i=0; i<numberOfElements; i++) dataPointer[i] = x;
//         return *this;
//     }
// //
// //  The number of array dimensions, elements, etc.
// //
//     int getNumberOfElements()   const { return numberOfElements;        }
//     int getNumberOfDimensions() const { return numberOfDimensions;      }
//     int getBase(const int i)    const { return dimension[i].getBase();  }
//     int getBound(const int i)   const { return dimension[i].getBound(); }
//     void consistencyCheck()     const { }
// //
// //  Use operator[] as if the array were one-dimensional, starting at index 0.
// //
//     Type& operator[](const int i) { return dataPointer[i]; }
//     const Type& operator[](const int i) const { return ((TrivialArray&)*this)[i]; }
// //
// //  Index the array using operator().
// //
//     Type& operator()(const int i0)
//       { assert(numberOfDimensions == 1); return basePointer[i0]; }
//     Type& operator()(const int i0, const int i1)
//       { assert(numberOfDimensions == 2); return basePointer[i0 + stride[1] * i1]; }
//     Type& operator()(const int i0, const int i1, const int i2)
//       { assert(numberOfDimensions == 3); return basePointer[i0 + stride[1] * i1 + stride[2] * i2]; }
//     Type& operator()(const int i0, const int i1, const int i2, const int i3)
//       { assert(numberOfDimensions == 4); return basePointer[i0 + stride[1] * i1 + stride[2] * i2 + stride[3] * i3]; }
//     Type& operator()(const int i0, const int i1, const int i2, const int i3, const int i4)
//       { assert(numberOfDimensions == 5); return basePointer[i0 + stride[1] * i1 + stride[2] * i2 + stride[3] * i3 + stride[4] * i4]; }
//     Type& operator()(const int i0, const int i1, const int i2, const int i3, const int i4, const int i5)
//       { assert(numberOfDimensions == 6); return basePointer[i0 + stride[1] * i1 + stride[2] * i2 + stride[3] * i3 + stride[4] * i4 + stride[5] * i5]; }
//     Type& operator()(const int i0, const int i1, const int i2, const int i3, const int i4, const int i5, const int i6)
//       { assert(numberOfDimensions == 7); return basePointer[i0 + stride[1] * i1 + stride[2] * i2 + stride[3] * i3 + stride[4] * i4 + stride[5] * i5
//                                                                + stride[6] * i6]; }
//     Type& operator()(const int i0, const int i1, const int i2, const int i3, const int i4, const int i5, const int i6, const int i7)
//       { assert(numberOfDimensions == 8); return basePointer[i0 + stride[1] * i1 + stride[2] * i2 + stride[3] * i3 + stride[4] * i4 + stride[5] * i5
//                                                                + stride[6] * i6 + stride[7] * i7]; }
//     Type& operator()(const int i0, const int i1, const int i2, const int i3, const int i4, const int i5, const int i6, const int i7, const int i8)
//       { assert(numberOfDimensions == 9); return basePointer[i0 + stride[1] * i1 + stride[2] * i2 + stride[3] * i3 + stride[4] * i4 + stride[5] * i5
//                                                                + stride[6] * i6 + stride[7] * i7 + stride[8] * i8]; }
//     Type& operator()(const int i0, const int i1, const int i2, const int i3, const int i4, const int i5, const int i6, const int i7, const int i8, const int i9)
//       { assert(numberOfDimensions ==10); return basePointer[i0 + stride[1] * i1 + stride[2] * i2 + stride[3] * i3 + stride[4] * i4 + stride[5] * i5
//                                                                + stride[6] * i6 + stride[7] * i7 + stride[8] * i8 + stride[9] * i9]; }
//     const Type& operator()(const int i0) const
//       { return ((TrivialArray&)*this)(i0); }
//     const Type& operator()(const int i0, const int i1) const
//       { return ((TrivialArray&)*this)(i0, i1); }
//     const Type& operator()(const int i0, const int i1, const int i2) const
//       { return ((TrivialArray&)*this)(i0, i1, i2); }
//     const Type& operator()(const int i0, const int i1, const int i2, const int i3) const
//       { return ((TrivialArray&)*this)(i0, i1, i2, i3); }
//     const Type& operator()(const int i0, const int i1, const int i2, const int i3, const int i4) const
//       { return ((TrivialArray&)*this)(i0, i1, i2, i3, i4); }
//     const Type& operator()(const int i0, const int i1, const int i2, const int i3, const int i4, const int i5) const
//       { return ((TrivialArray&)*this)(i0, i1, i2, i3, i4, i5); }
//     const Type& operator()(const int i0, const int i1, const int i2, const int i3, const int i4, const int i5, const int i6) const
//       { return ((TrivialArray&)*this)(i0, i1, i2, i3, i4, i5, i6); }
//     const Type& operator()(const int i0, const int i1, const int i2, const int i3, const int i4, const int i5, const int i6, const int i7) const
//       { return ((TrivialArray&)*this)(i0, i1, i2, i3, i4, i5, i6, i7); }
//     const Type& operator()(const int i0, const int i1, const int i2, const int i3, const int i4, const int i5, const int i6, const int i7, const int i8) const
//       { return ((TrivialArray&)*this)(i0, i1, i2, i3, i4, i5, i6, i7, i8); }
//     const Type& operator()(const int i0, const int i1, const int i2, const int i3, const int i4, const int i5, const int i6, const int i7, const int i8,
//                            const int i9) const
//       { return ((TrivialArray&)*this)(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9); }
//   private:
//     static int min0(const int i, const int j) { return i < j ? i : j; }
//     static int max0(const int i, const int j) { return i > j ? i : j; }
// //
// //  Private data:
// //
//     int   numberOfElements;
//     int   numberOfDimensions;
//     Dim   dimension[MAXDIM];
//     int   stride[MAXDIM];
//     Type *dataPointer, *basePointer;
// #undef DIM_ARGUMENTS
// #undef DIM_ARGS
// #undef X_DIM_ARGS
// #undef DIM_DECLARATIONS
};

#endif // TrivialArray_
