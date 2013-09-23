template<class T>
void addMembersToIndex(T *def)
   {
     int *cd;

  // This call to the template causes the template using "int" to be instantiated.
     addMembersToIndex(cd);
   }

void writeDirTreeNode()
   {
     float *fd;

  // This call to the template using "float" causes the template using "int" to be instantiated (in the instantiated template).
     addMembersToIndex(fd);
   }

