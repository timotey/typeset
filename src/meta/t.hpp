#ifndef META_T_HPP__
#define META_T_HPP__
#include<type_traits>
#define T_V(x) t_v<std::decay_t<decltype(x)>>
template<template<class>class T>
constexpr auto template_ = [](auto&&... xs){
    return T(FWD(xs)...);
};

template<class T>
struct t{
    constexpr t(){}
    constexpr t(T&&){}
    constexpr t(T const&){}
    template<class U>
    constexpr bool operator ==(t<U>){return false;}
    constexpr bool operator ==(t   ){return true;}
    constexpr bool operator !=(auto v){return !(*this == v);}
    T operator*() const;
    T* operator->() const;
};
template<class T>
constexpr auto t_v = t<T>{};
#endif //META_T_HPP__
