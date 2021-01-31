#pragma once

#include <type_traits>

template<template<auto...> class Container, class ToTest>
constexpr bool is_instance = false;


template<template<auto...> class Container, auto... Ts>
constexpr bool is_instance<Container, Container<Ts...>> = true;


template<template<auto...> class Container, class... ToTest>
constexpr bool are_instances = (is_instance<Container, ToTest> && ...);



template<unsigned int n, auto... Ts>
requires (sizeof...(Ts) > n)
struct ExtractNthArg {

    template<int i, auto first, auto... rest>
    struct Step {
        static constexpr auto value = Step<i + 1, rest...>::value;
    };

    template<auto first, auto... rest>
    struct Step<n, first, rest...> {
        static constexpr auto value = first;
    };

    static constexpr auto value = Step<0, Ts...>::value;
};

template<auto... Ts>
static constexpr auto extract_nth_arg = ExtractNthArg<Ts...>::value;
