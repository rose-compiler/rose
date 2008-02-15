// 'using namespace std' before 'namespace std'

//  [ This may be a gcc extension, though gcc -pedantic accepts it. --Karl ]

// originally found in package amsynth

// ERR-MATCH: could not find namespace `(?:\::)?std'

using namespace std;

using namespace ::std;

//ERROR(1): using namespace BogusNamespace;
