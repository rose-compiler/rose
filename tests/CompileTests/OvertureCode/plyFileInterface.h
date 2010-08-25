//  C++ wrapper for PLY polygonal file interface
//   Petri Fast, 2001
//

#include <string.h>
#include "Overture.h"

enum PlyFileReadWriteFlag { READ_PLY_FILE, WRITE_PLY_FILE };
enum PlyFileType          { ASCII_PLY_FILE=1,  
			    BE_BINARY_PLY_FILE=2,  // big-endian
			    LE_BINARY_PLY_FILE=3}; // little-endian

// forward definitions
struct PlyFile;  
struct PlyProperty;

class PlyFileInterface
{
public:
  PlyFileInterface();
  ~PlyFileInterface();

  int       openFile(const aString &fname, 
                     PlyFileReadWriteFlag readWrite=READ_PLY_FILE);
  void      closeFile();
  
  void      readFile(intArray &elems, intArray &tags, realArray &xyz,
                     int &nnode00, int &nelem00, int &ddim00, int &rdim00);

  void      writeFile(); // - " -

  PlyFile             *ply;
  PlyFileType         fileType;

  int rdim;
  int ddim;
  int nnode, nelem;
  int nemax;  

private:
  int nelems;
  char **elist;
  float version;


  PlyProperty **plist;
  int nprops;
  int num_elems;

  int num_comments;
  char **comments;
  int num_obj_info;
  char **obj_info;

  //char *elem_name;
};

