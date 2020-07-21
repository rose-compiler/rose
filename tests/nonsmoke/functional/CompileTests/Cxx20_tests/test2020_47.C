std::list<int> l = {3,-1,10};
std::sort(l.begin(), l.end()); 
//Typical compiler diagnostic without concepts:
//  invalid operands to binary expression ('std::_List_iterator<int>' and
//  'std::_List_iterator<int>')
//                           std::__lg(__last - __first) * 2);
//                                     ~~~~~~ ^ ~~~~~~~
// ... 50 lines of output ...
//
//Typical compiler diagnostic with concepts:
//  error: cannot call std::sort with std::_List_iterator<int>
//  note:  concept RandomAccessIterator<std::_List_iterator<int>> was not satisfied

