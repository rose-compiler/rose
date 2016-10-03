// #include <vector>
namespace std
   {
//   template<typename _Tp> class allocator {};

//   template<typename _Tp, typename _Alloc = std::allocator<_Tp> >
     template<typename _Tp>
     class vector
        {
          public:
               vector();

               class const_iterator
                  {
                    public:
                      // const_iterator & operator+ (const_iterator & X);
                      // const_iterator & operator- (const_iterator & X);
                     friend const_iterator operator+ (const_iterator X, const_iterator Y);
                     friend const_iterator operator- (const_iterator X, const_iterator Y);
                  // friend const_iterator & operator- (_Tp X, const_iterator & Y);
                     _Tp & operator* ();
                  };

               const_iterator begin();
            // friend _Tp operator* (const_iterator X);
        };
   }


namespace {

typedef std::vector<std::vector<int> > SpeciesMap;

class ReplaceMatOpCommand
   {
     private:
       // virtual bool replaceMaterials(unsigned zone_index) const;
          virtual bool replaceMaterials(unsigned zone_index);

          std::vector<int> m_materials_to_replace;
          std::vector<int> m_replacement_materials;
   };

// bool ReplaceMatOpCommand::replaceMaterials(unsigned zone_index) const
bool ReplaceMatOpCommand::replaceMaterials(unsigned zone_index)
   {
     typedef std::vector<int>::const_iterator VIIter;

     VIIter mat_iter;

  // Bug: is unparsed as: "int replacement_material =  * ((this) -> m_replacement_materials . begin() + mat_iter-(this) -> m_materials_to_replace . begin());"
  // This appears to be an operator presidence issue between operator+() and operator-().
  // And the original code rhs was: *(m_replacement_materials.begin() + (mat_iter - m_materials_to_replace.begin()));
     int replacement_material = *(m_replacement_materials.begin() + (mat_iter - m_materials_to_replace.begin()));

     return false;
   }

}

