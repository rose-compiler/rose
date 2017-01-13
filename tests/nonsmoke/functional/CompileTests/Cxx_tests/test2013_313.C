// This example comes from: http://www.gotw.ca/publications/mill17.htm

// Important Morals

// If you're like me, the first time you see this you'll ask the question: "Hmm. But it seems to me that I went and specifically wrote a specialization for the case when the parameter is an int*, and it is an int* which is an exact match, so shouldn't my specialization always get used?" That, alas, is a mistake: If you want to be sure it will always be used in the case of exact match, that's what a plain old function is for -- so just make it a function instead of a specialization.

// The rationale for why specializations don't participate in overloading is simple, once explained, because the surprise factor is exactly the reverse: The standards committee felt it would be surprising that, just because you happened to write a specialization for a particular template, that it would in any way change which template gets used. Under that rationale, and since we already have a way of making sure our version gets used if that's what we want (we just make it a function, not a specialization), we can understand more clearly why specializations don't affect which template gets selected.

//    Moral #1: If you want to customize a function base template and want that customization to participate in overload resolution (or, to always be used in the case of exact match), make it a plain old function, not a specialization. And, if you do provide overloads, avoid also providing specializations.

// But what if you're the one who's writing, not just using, a function template? Can you do better and avoid this (and other) problem(s) up front, for yourself and for your users? Indeed you can:

//    Moral #2: If you're writing a function base template, prefer to write it as a single function template that should never be specialized or overloaded, and then implement the function template entirely as a simple handoff to a class template containing a static function with the same signature. Everyone can specialize that -- both fully and partially, and without affecting the results of overload resolution.

// Example 4: Illustrating Moral #2 
// 
template<class T> 
struct FImpl;

template<class T> 
void f( T t ) { FImpl<T>::f( t ); } // users, don't touch this!

template<class T> 
struct FImpl 
{ 
  static void f( T t ); // users, go ahead and specialize this 
};
