// t0478.cc
// -> is rewritten to operator-> only when template is instantiated

template < class ACE_CB >
void open (void)
{
  typename ACE_CB::MALLOC_HEADER_PTR freep_;
  &(freep_->next_block_);
}


template < class CONCRETE >
class ACE_Based_Pointer {
public:
  CONCRETE *operator-> (void);
};

class ACE_PI_Control_Block {
public:
  class ACE_Malloc_Header;

  typedef ACE_Based_Pointer < ACE_Malloc_Header > MALLOC_HEADER_PTR;

  class ACE_Malloc_Header {
  public:
    MALLOC_HEADER_PTR next_block_;
  };
};

void foo()
{
  open<ACE_PI_Control_Block>();
}

