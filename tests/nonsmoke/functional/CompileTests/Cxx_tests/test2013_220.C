#include <vector>

class FieldDescriptor {};
   
class FieldGroup 
   {
     public:

       // Append the fields in 'other' to this group.
          void Append(const FieldGroup& other) 
             {
           //  preferred_location_ = (preferred_location_ * fields_.size() + (other.preferred_location_ * other.fields_.size())) / (fields_.size() + other.fields_.size());
               fields_.insert(fields_.end(), other.fields_.begin(), other.fields_.end());
             }

     private:
          std::vector<const FieldDescriptor*> fields_;
   };

