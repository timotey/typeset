#ifndef META_FUNCTIONAL_HPP__
#define META_FUNCTIONAL_HPP__
#include<utility>
#define FWD(x) std::forward<std::remove_reference_t<decltype(x)>>(x)
template<template<class...> class T>
auto create = [](auto&&... vs){
    return T(FWD(vs)...);
};
auto overload = []<class... As>(As&&... a) {
    struct x: As... {
        using As::operator()...;
    };
    return x{a...};
};

auto bind_front = [](auto&& f, auto&&... xs){
    return [&](auto&&... ys){return FWD(f)(FWD(xs)..., FWD(ys)...);};
};

template <typename F>
struct fix_t {
    F f;
    fix_t(F&& _f):f(std::move(_f)){}
    constexpr auto operator()(auto&& ...x)const
        ->decltype(f(*this, FWD(x)...))
    { return f(*this, FWD(x)...); }
    constexpr auto operator()(auto&& ...x)
        ->decltype(f(*this, FWD(x)...))
    { return f(*this, FWD(x)...); }
};

auto fix = create<fix_t>;

template<class T> struct foldl_t{
    T fn;
    auto&& operator()(auto&& init, auto&& next, auto&&... vs){
        return (*this)(fn(init, FWD(next)), FWD(vs)...); }
    auto&& operator()(auto&& init){
        return FWD(init); }
};
template<class... Ts>
foldl_t(Ts&&...)->foldl_t<Ts&&...>;
auto foldl = [](auto&& f){
    return [f=FWD(f)](auto&& init)->decltype(auto){
        return [f=FWD(f), init = FWD(init)](auto&&... vs)->decltype(auto){
            return foldl_t{FWD(f)}(FWD(init), FWD(vs)...);
        };
    };
};

template<class T> struct foldr_t{
    T fn;
    auto operator()(auto&& init, auto&& next, auto&&... vs){
        return fn(*this(FWD(init), FWD(vs)...), next); }
    auto operator()(auto&& init){
        return FWD(init); }
};
template<class... Ts>
foldr_t(Ts...)->foldr_t<Ts...>;
auto foldr = [](auto&& f){
    return [f=FWD(f)](auto&& init){
        return [f=FWD(f), init = FWD(init)](auto&&... vs){
            return foldr_t(FWD(f))(FWD(init), FWD(vs)...); }; }; };

//auto operator&(auto&& a, auto&& b){
//    return [a=FWD(a), b=FWD(b)](auto&&... xs){ return FWD(b)(FWD(a)(FWD(xs)...));};
//}

#endif //META_FUNCTIONAL_HPP__
