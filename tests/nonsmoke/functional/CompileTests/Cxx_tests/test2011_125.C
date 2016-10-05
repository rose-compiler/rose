// Name qualification bug submitted by Faizur
namespace std
{
  class type_info
  {
  };
}

struct Handler
{
  virtual void Handle_forward()
  {
      typeid(*this);
  }
};
