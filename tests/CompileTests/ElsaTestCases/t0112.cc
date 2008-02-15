// t0112.cc
// some random problem..   


template <class CharT>
class SomeBase {};

template <class CharT>
class nsSharedBufferHandle : SomeBase<CharT>
  {
      void
      AcquireReference() const
        {
          nsSharedBufferHandle<CharT>* mutable_this = 
            const_cast<  nsSharedBufferHandle<CharT>*  >(  this ) ;
          mutable_this->set_refcount( get_refcount()+1 );
        }

      void ReleaseReference() const;

      bool
      IsReferenced() const
        {
          return get_refcount() != 0;
        }

      void StorageLength( int aNewStorageLength )
        {
        }

      int
      StorageLength() const
        {
        }

    protected:
      int  mFlags;
      int mStorageLength;

      int
      get_refcount() const
        {
          return 1;
        }

      int
      set_refcount( int aNewRefCount )
        {
          return 1;
        }
  };
