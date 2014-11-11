// headers
#define STACK_ASSIGN_RESTORE_HEADER(type,typename) \
 private:\
  stack<pair<type*, type> > stack_##typename;\
  void restore_##typename();\
 public:\
  type assign(type* address, type value);\
  type* avpush(type* address); \

// implementations
#define ASSIGN_RESTORE_IMPLEMENTATION(type,mytypename,typeenum) \
type Backstroke::RunTimeSystem::assign(type* address, type value) { \
 if(!is_stack_ptr(address)) {\
  currentEventRecord->stack_bitype.push(typeenum);\
  stack_##mytypename.push(make_pair(address,*address));\
 }\
  return *address=value;\
}\
\
void Backstroke::RunTimeSystem::restore_##mytypename() {\
  pair<type*,type> p=stack_##mytypename.top();\
  stack_##mytypename.pop();\
  *(p.first)=p.second;\
}\
type* Backstroke::RunTimeSystem::avpush(type* address) { \
 if(!is_stack_ptr(address)) {\
  currentEventRecord->stack_bitype.push(typeenum);\
  stack_##mytypename.push(make_pair(address,*address));\
 }\
  return address;\
}\
\


#define CASE_ENUM_RESTORE(enumname,mytypename) \
  case BITYPE_##enumname: restore_##mytypename();break

#define CASE_ENUM_SIZEOF(enumname,mytypename) \
  case BITYPE_##enumname: return sizeof(mytypename)
