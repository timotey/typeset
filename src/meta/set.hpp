#ifndef META_SET_HPP__
#define META_SET_HPP__
#include <type_traits>
#include<utility>
#include<tuple>
#include"t.hpp"
#include"functional.hpp"

using std::size_t;

template<class T, class... Ts>
struct contains_v;

template<class T, class U, class... Ts>
struct contains_v<T, U, Ts...>{
    static constexpr bool value = contains_v<T, Ts...>::value;
};
template<class T, class... Ts>
struct contains_v<T, T, Ts...>{
    static constexpr bool value = true;
};
template<class T>
struct contains_v<T>{
    static constexpr bool value = false;
};

template<class... Ts> struct unique_v;
template<class T, class... Ts> struct unique_v<T, Ts...>{
    static constexpr bool value = !contains_v<T, Ts...>::value && unique_v<Ts...>::value; };
template<> struct unique_v<>{
    static constexpr bool value = true; };

template<bool op, class Tuple1, class Tuple2,
    size_t CurrentIdx = 0,
    class SequenceAcc = std::index_sequence<>
>struct index_sequence_for_set_op;

template<bool op, class Tuple1, class Tuple2>
using make_index_sequence_for_set_op = typename index_sequence_for_set_op<op, Tuple1, Tuple2>::type;

template<bool op, class... Us, size_t CurrIdx, size_t... Idxs>
struct index_sequence_for_set_op<
    op, std::tuple<>, std::tuple<Us...>,
    CurrIdx, std::index_sequence<Idxs...>>{
    using type = std::index_sequence<Idxs...>;
};

template<bool cond, class T, class U> struct conditional;
template<class T, class U> struct conditional<true, T, U>{using type = T;};
template<class T, class U> struct conditional<false,T, U>{using type = U;};

template<bool op, class T, class... Ts, class... Us, size_t CurrIdx, size_t... Idxs>
struct index_sequence_for_set_op<op, std::tuple<T, Ts...>, std::tuple<Us...>, CurrIdx, std::index_sequence<Idxs...>>{
    using type = typename index_sequence_for_set_op<
        op,
        std::tuple<Ts...>,
        std::tuple<Us...>,
        CurrIdx+1,
        typename std::conditional<
            op==contains_v<T, Us...>::value,
            std::index_sequence<Idxs...>,
            std::index_sequence<CurrIdx, Idxs...>
        >::type
    >::type;
};

template<class... Ts>
struct set_t;
template<class... Ts>
set_t(Ts&&...) -> set_t<Ts...>;
inline constexpr auto set = create<set_t>;
template<class... Ts>
struct set_t:public std::tuple<Ts...>{
    template<class... Us>
    explicit set_t(Us&&... ts) requires(unique_v<Ts...>::value) :std::tuple<Ts...>(FWD(ts)...){}
    template<class T>
    constexpr auto insert(T v){
                return (*this)([&v](auto...vs){return set(v,vs...);});
    }
    template<class... Us>
    auto operator|(set_t<Us...>const& other)const&{
        using T1 = std::tuple<Ts...>; using T2 = std::tuple<Us...>;
        return [&]<size_t... I1, size_t... I2>(std::index_sequence<I1...>, std::index_sequence<I2...>){
            return set_t<Ts..., std::tuple_element_t<I2, T2>...>(std::get<I1>(*this)..., std::get<I2>(other)...);
        }(std::make_index_sequence<sizeof...(Ts)>{}, make_index_sequence_for_set_op<true, T2, T1>{});
    }
    template<class... Us>
    auto operator|(set_t<Us...>const& other)&&{
        using T1 = std::tuple<Ts...>; using T2 = std::tuple<Us...>;
        return [&]<size_t... I1, size_t... I2>(std::index_sequence<I1...>, std::index_sequence<I2...>){
            return set_t<Ts..., std::tuple_element_t<I2, T2>...>(std::get<I1>(std::move(*this))..., std::get<I2>(other)...);
        }(std::make_index_sequence<sizeof...(Ts)>{}, make_index_sequence_for_set_op<true, T2, T1>{});
    }
    template<class... Us>
    auto operator|(set_t<Us...>&& other)const&{
        using T1 = std::tuple<Ts...>; using T2 = std::tuple<Us...>;
        return [&]<size_t... I1, size_t... I2>(std::index_sequence<I1...>, std::index_sequence<I2...>){
            return set_t<Ts..., std::tuple_element_t<I2, T2>...>(std::get<I1>(*this)..., std::get<I2>(std::move(other))...);
        }(std::make_index_sequence<sizeof...(Ts)>{}, make_index_sequence_for_set_op<true, T2, T1>{});
    }
    template<class... Us>
    auto operator|(set_t<Us...>&& other)&&{
        using T1 = std::tuple<Ts...>; using T2 = std::tuple<Us...>;
        return [&]<size_t... I1, size_t... I2>(std::index_sequence<I1...>, std::index_sequence<I2...>){
            return set_t<Ts..., std::tuple_element_t<I2, T2>...>(std::get<I1>(std::move(*this))..., std::get<I2>(std::move(other))...);
        }(std::make_index_sequence<sizeof...(Ts)>{}, make_index_sequence_for_set_op<true, T2, T1>{});
    }
    template<class... Us>
    auto operator&(set_t<Us...>const&)const&{
        using T1 = std::tuple<Ts...>; using T2 = std::tuple<Us...>;
        return [&]<size_t... I>(std::index_sequence<I...>){
            return set_t<std::tuple_element_t<I, T1>...>(std::get<I>(*this)...);
        }(make_index_sequence_for_set_op<false, T1, T2>{});
    }
    template<class... Us>
    auto operator&(set_t<Us...>const&)&&{
        using T1 = std::tuple<Ts...>; using T2 = std::tuple<Us...>;
        return [&]<size_t... I>(std::index_sequence<I...>){
            return set_t<std::tuple_element_t<I, T1>...>(std::get<I>(std::move(*this))...);
        }(make_index_sequence_for_set_op<false, T1, T2>{});
    }
    template<class... Us>
    auto operator/(set_t<Us...>const&)const&{
        using T1 = std::tuple<Ts...>; using T2 = std::tuple<Us...>;
        return [&]<size_t... I>(std::index_sequence<I...>){
            return set_t<std::tuple_element_t<I, T1>...>(std::get<I>(*this)...);
        }(make_index_sequence_for_set_op<true, T1, T2>{});
    }
    template<class... Us>
    auto operator/(set_t<Us...>const&)&&{
        using T1 = std::tuple<Ts...>; using T2 = std::tuple<Us...>;
        return [&]<size_t... I>(std::index_sequence<I...>){
            return set_t<std::tuple_element_t<I, T1>...>(std::get<I>(std::move(*this))...);
        }(make_index_sequence_for_set_op<true, T1, T2>{});
    }
};

#endif //META_SET_HPP__
