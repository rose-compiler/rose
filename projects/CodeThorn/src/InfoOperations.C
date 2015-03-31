template<typename T>
void printAttributes(Labeler* labeler, VariableIdMapping* vim, string attributeName) {
  long labelNum=labeler->numberOfLabels();
  for(long i=0;i<labelNum;++i) {
    Label lab=i;
    SgNode* node=labeler->getNode(i);
    //    cout<<"@Label "<<lab<<":";
    T* node0=dynamic_cast<T*>(node->getAttribute(attributeName));
    if(node0)
      node0->toStream(cout,vim);
    else
      cout<<" none.";
    cout<<endl;
  }
}

void printRoseInfo(SgProject* project) {
  project->display("PROJECT NODE");
  int fileNum=project->numberOfFiles();
  for(int i=0;i<fileNum;i++) {
    std::stringstream ss;
    SgFile* file=(*project)[i];
    ROSE_ASSERT(file);
    ss<<"FILE NODE Nr. "<<i;
    file->display(ss.str());
  }
}

