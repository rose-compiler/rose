// operator & can have just one argument, as it isn't just bit-and it
// is also address-of
class EST_Chunk {
    EST_Chunk *operator & ();
};

EST_Chunk *EST_Chunk::operator & ()
{
  return this;
}
