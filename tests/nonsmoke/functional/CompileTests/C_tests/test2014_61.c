


typedef void* gpointer;

typedef int mword;

// void par_copy_object_no_checks (char *destination, MonoVTable *vt, void *obj, mword objsize, SgenGrayQueue *queue)
void par_copy_object_no_checks (char *destination, void *obj, mword objsize )
   {
#ifdef __GNUC__
     static const void *copy_labels [] = { &&LAB_0, &&LAB_1, &&LAB_2, &&LAB_3, &&LAB_4, &&LAB_5, &&LAB_6, &&LAB_7, &&LAB_8 };

     if (objsize <= sizeof (gpointer) * 8) 
        {
          mword *dest = (mword*)destination;
          goto *copy_labels [objsize / sizeof (gpointer)];
          LAB_8:
               (dest) [7] = ((mword*)obj) [7];
          LAB_7:
               (dest) [6] = ((mword*)obj) [6];
          LAB_6:
               (dest) [5] = ((mword*)obj) [5];
          LAB_5:
               (dest) [4] = ((mword*)obj) [4];
          LAB_4:
               (dest) [3] = ((mword*)obj) [3];
          LAB_3:
               (dest) [2] = ((mword*)obj) [2];
          LAB_2:
               (dest) [1] = ((mword*)obj) [1];
          LAB_1:
               ;
          LAB_0:
               ;
        }
#endif
   }
