// test outlining a code block with zero variables referenced.
//
void noop()
{}

void A ()
{ 
#pragma rose_outline
  { 
    noop();
  }
}


