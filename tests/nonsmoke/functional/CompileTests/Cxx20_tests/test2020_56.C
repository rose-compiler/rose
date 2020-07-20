template<class T> constexpr bool is_meowable = true;
template<class T> constexpr bool is_cat = true;
 
template<class T>
concept Meowable = is_meowable<T>;
 
template<class T>
concept BadMeowableCat = is_meowable<T> && is_cat<T>;
 
template<class T>
concept GoodMeowableCat = Meowable<T> && is_cat<T>;
 
template<Meowable T>
void f1(T); // #1
 
template<BadMeowableCat T>
void f1(T); // #2
 
template<Meowable T>
void f2(T); // #3
 
template<GoodMeowableCat T>
void f2(T); // #4
 
void g(){
    f1(0); // error, ambiguous:
           // the is_meowable<T> in Meowable and BadMeowableCat forms distinct
           // atomic constraints that are not identical (and so do not subsume each other)
 
    f2(0); // OK, calls #4, more constrained than #3
           // GoodMeowableCat got its is_meowable<T> from Meowable
}

