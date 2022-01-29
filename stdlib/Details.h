#ifndef OS_DETAILS_H
#define OS_DETAILS_H

template <class T>
struct type_identity {
    using type = T;
};

//https://bugs.llvm.org/show_bug.cgi?id=27798
template<typename _Tp, typename _Up = _Tp&&>
_Up
__declval(int);

template<typename _Tp>
_Tp
__declval(long);

template<typename _Tp>
auto declval() noexcept -> decltype(__declval<_Tp>(0));

template<class T> struct remove_reference      {using type = T;};
template<class T> struct remove_reference<T&>  {using type = T;};
template<class T> struct remove_reference<T&&> {using type = T;};

template<typename T>
using remove_reference_t = typename remove_reference<T>::type;

template<class T>
constexpr T&& forward( remove_reference_t<T>&& t ) noexcept
{
    return static_cast<T&&>(t);
}

//http://thbecker.net/articles/rvalue_references/section_08.html
template<class T>
typename remove_reference<T>::type&&
move(T&& a) noexcept
{
    typedef typename remove_reference<T>::type&& RvalRef;
    return static_cast<RvalRef>(a);
}

#endif //OS_DETAILS_H
