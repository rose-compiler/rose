#include<vector>

#define BLOCKSIZE 32

using namespace std;

class Point 
   {
     public:
          Point();
          Point operator*(int x);
          Point operator+(Point x);
   };

Point getOnes();

class Box 
   {
     public:
          Box();
          Box(Point lc, Point hc);
   };



class BoxLayout 
   {
     public:
          BoxLayout();
          BoxLayout(int a_M, const vector<Point>& a_points);

       // generate (2^a_M)^DIM boxes of equal size
          BoxLayout(int a_M);
         ~BoxLayout();
 
//        const RectMDArray<bool>& getBitmap() const;

       // fetch the low corner from the lowest corner box and high corner from the highest corner box
          const Box& getDomain() const;
      /// returns the patch corresponding to this point in the bitmap. *bi, where bi is a 
      /// BoxIterator, is an appropriate argument.
          inline Box operator[](const Point& a_pt) const
             {
               Point lc   = a_pt * BLOCKSIZE;

            // DQ (2/6/2015): This is a bug in ROSE that I think was previously fixed to support the laplacian.cpp file.
            // Point incr = getOnes()*(BLOCKSIZE-1);
               Point incr = getOnes()*(BLOCKSIZE);
               Point hc   = lc + incr;
               return Box(lc,hc);
             };
#if 0
          int getPatchIndex(const Point& a_point) const;
          int getPatchIndexFromBitpoint(const Point& a_point) const;
          Point getCornerFromBitpoint(const Point& a_point);
          bool neighborExists(const Point& a_p, const int a_dir, const int a_face);
          vector<Point>& getPatchLocs() const;
          int size(){return m_getPatches->size();};
          Point getNeighbor(const Point& a_p, const Point& a_dir);
       // there has to be a better way of doing this
          Point getCorner(const int index);
          Box shift(const Box& a_bx,const Point& a_pt);

     private:
       // BoxLayout* m_boxLayoutPtr;
          Box m_domain;
          Box m_bitbox;

          shared_ptr<RectMDArray<bool>> m_bitmap;
          shared_ptr<vector<Point>> m_patchlocs;
          shared_ptr<map<Point, int >> m_getPatches;
#endif
   };

