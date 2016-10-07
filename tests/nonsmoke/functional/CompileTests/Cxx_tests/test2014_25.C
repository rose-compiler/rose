void IntVectSet::clearStaticMemory()
{
  TreeIntVectSet::treeNodePool->clear();
  TreeIntVectSet::index.clear();
  TreeIntVectSet::parents.clear();
  TreeIntVectSet::boxes.clear();
  TreeIntVectSet::bufferOffset.clear();
}

Unparsed as:
void IntVectSet::clearStaticMemory()
{
TreeIntVectSet::treeNodePool ->  clear ();
index . clear();
TreeIntVectSet::parents . clear();
boxes . clear();
TreeIntVectSet::bufferOffset . clear();
}

