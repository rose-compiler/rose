Face::
Face(int id_, int v1_, int v2_, int z1_, int z2_)
{
  id = id_;
  v1id = v1_;
  v2id = v2_;
  z1id = z1_;
  z2id = z2_;
}

int
Face::
getID() const { return id; }

int
Face::
getV1ID() const { return v1id ; }

int
Face::
getV2ID() const { return v2id ; }

int
Face::
getZ1ID() const { return z1id ; }

int
Face::
getZ2ID() const { return z2id ; }

void
Face::
setZ2ID(int z2id_) { z2id = z2id_; }
