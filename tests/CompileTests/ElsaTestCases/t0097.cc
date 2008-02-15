// t0097.cc
// mismatch provoked by confusing qualification?
        
class nsFrameUtil {
  //struct Node;
  struct Tag;
  struct Node {
    static Node* Read(Tag* aTag);
  };
  struct Tag {};
};

nsFrameUtil::Node*
nsFrameUtil::Node::Read(Tag* tag)
{
}

