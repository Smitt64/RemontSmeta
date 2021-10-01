#ifndef TYPELIST_HPP
#define TYPELIST_HPP

#include <type_traits>

namespace internal
{
    struct Void
    {
    };
} // internal

template<typename ...Args>
struct TypeList
{
    typedef internal::Void Head;
    typedef internal::Void Tail;
};

typedef TypeList<> EmptyTypeList;

template<typename H, typename ...T>
struct TypeList<H, T...>
{
    typedef H Head;
    typedef TypeList<T...> Tail;
};

template<typename TL>
struct IsEmpty :
    std::true_type
{
};

template<>
struct IsEmpty<TypeList<internal::Void, internal::Void>> :
    std::true_type
{
};

template<typename ...Args>
struct IsEmpty<TypeList<Args...>> :
    std::integral_constant<bool,
        std::is_same<typename TypeList<Args...>::Head, internal::Void>::value &&
        IsEmpty<typename TypeList<Args...>::Tail>::value>
{
};

template<typename T, typename TL>
struct Contains :
    std::false_type
{
};

/*template<typename ...Args>
struct Contains<internal::Void, Args...> :
    std::false_type
{
};

template<typename T, typename ...Args>
struct Contains<T, TypeList<Args...>> :
    std::integral_constant<bool,
        std::is_same<typename TypeList<Args...>::Head, T>::value ||
        Contains<T, typename TypeList<Args...>::Tail>::value
        >
{
};*/

template<typename TL>
struct Length : std::integral_constant<unsigned int, 0>
{
};

template<typename ...Args>
struct Length<TypeList<Args...>> :
    std::integral_constant<unsigned int,
        IsEmpty<TypeList<Args...>>::value
            ? 0
            : 1 + Length<typename TypeList<Args...>::Tail>::value>
{
};

template<unsigned int N, typename TL>
struct TypeAt
{
    typedef internal::Void type;
};

template<typename TOrTL2, typename TL>
struct Append
{
};

template<typename T, typename ...Args>
struct Append<T, TypeList<Args...>>
{
    typedef TypeList<Args..., T> type;
};

template<typename ...Args1, typename ...Args2>
struct Append<TypeList<Args1...>, TypeList<Args2...>>
{
    typedef TypeList<Args2..., Args1...> type;
};

template<typename T, typename TL>
struct Add
{
};

template<typename T, typename ...Args>
struct Add<T, TypeList<Args...>>
{
    typedef TypeList<Args..., T> type;
};
#endif // TYPELIST_HPP
