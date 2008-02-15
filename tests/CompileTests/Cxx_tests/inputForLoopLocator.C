
#if 0
Type 1 case:
   for (typename MeshType::ZoneIterator zoneItr = mesh.ownedZoneBegin(); zoneItr != mesh.ownedZoneEnd(); ++zoneItr) 
        mOldZoneVolumes[*zoneItr] = ...


Type 2 cases:
   for (typename MeshType::ZoneIterator zoneItr = mesh.ownedZoneBegin(); zoneItr != mesh.ownedZoneEnd(); ++zoneItr) 
       mOldZoneVolumes[*zoneItr] = zoneItr->getVolume();

   for (typename Geometry::Field<MeshType, Zone, Scalar>::Index i = mOldZoneVolumes.startIndex(); i < mOldZoneVolumes.endIndex(); ++i) 
       mOldZoneVolumes[i] = zoneItr->getVolume();

Type 3 case (cannot convert these):
   for (typename MeshType::ZoneIterator zoneItr = mesh.ownedZoneBegin(); zoneItr != mesh.ownedZoneEnd(); ++zoneItr) 
      foo(zoneItr);
      zoneItr->foo();
#endif





class MeshType
   {
     public:
          class iterator
             {
               public:
                    iterator ();
                    iterator (int i);

                    int operator*();
                    iterator *operator->() const;

                    bool operator!=(iterator i);
                    iterator & operator++(int i);

                    int getVolume();
                    int getPosition();
             };
         
          typedef iterator ZoneIterator;
          typedef iterator EdgeIterator;

          int & operator[](int i);

         iterator begin();
         iterator end();
   };

// DQ (10/14/2006): This is required to make this work with GNU (at least version 3.3.2)
// void foobar(MeshType::iterator i);
void foobar(MeshType::ZoneIterator i);

void foo()
   {
     MeshType mesh;
     for (MeshType::ZoneIterator i = mesh.begin(); i != mesh.end(); i++) 
        {
       // type 0
        }

     for (MeshType::ZoneIterator i = mesh.begin(); i != mesh.end(); i++) 
        {
          mesh[*i];       // type 1
        }

     for (MeshType::ZoneIterator i = mesh.begin(); i != mesh.end(); i++) 
        {
          i->getVolume(); // type 2
        }

     for (MeshType::ZoneIterator i = mesh.begin(); i != mesh.end(); i++) 
        {
          ::foobar(i);      // type 3
        }
#if 1
     for (MeshType::ZoneIterator i = mesh.begin(); i != mesh.end(); i++) 
        {
          mesh[*i];       // type 1
          i->getVolume(); // type 2
          foobar(i);      // type 3
        }
#endif

     MeshType::ZoneIterator i;
     for (i = mesh.begin(); i != mesh.end(); i++) 
        {
       // type 0
        }

     for (i = mesh.begin(); i != mesh.end(); i++) 
        {
          mesh[*i];       // type 1
        }

     for (i = mesh.begin(); i != mesh.end(); i++) 
        {
          i->getVolume(); // type 2
        }

     for (i = mesh.begin(); i != mesh.end(); i++) 
        {
          foobar(i);      // type 3
        }

     for (i = mesh.begin(); i != mesh.end(); i++) 
        {
          mesh[*i];         // type 1
          i->getVolume();   // type 2
          i->getPosition(); // type 2
          foobar(i);        // type 3
        }

   }
