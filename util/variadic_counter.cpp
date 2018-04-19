#include <iostream>
#include <type_traits>

template<typename Base, typename Arg, typename Enable = void>
struct counter_increment
{
    constexpr static int count = 0;
};

template<typename Base, typename Arg>
struct counter_increment<Base, Arg, typename std::enable_if< std::is_base_of<Base, Arg>::value>::type >
{
    constexpr static int count = 1;
};

template<typename T, typename ...Args>
struct variadic_counter;

template<typename T>
struct variadic_counter<T>
{
    constexpr static int count = 0;
};

template<typename T, typename Arg1, typename ...Args>
struct variadic_counter<T, Arg1, Args...>
{
    constexpr static int count = variadic_counter<T, Args...>::count + counter_increment<T, Arg1>::count;
};

class tag {};
class A1 : tag {};
class A2 : tag {};
class other_tag {};
class B1 : other_tag {};
class B2 : other_tag {};

int main(int argc, char ** argv)
{
    static_assert( variadic_counter<tag, A2>::count == 1, "");
    static_assert( variadic_counter<tag, A1, A2>::count == 2, "");
    static_assert( variadic_counter<tag, B1, A2>::count == 1, "");
    static_assert( variadic_counter<tag, A1, A2, tag, other_tag>::count == 3, "");
    static_assert( variadic_counter<tag, B1>::count == 0, "");
    static_assert( variadic_counter<tag, B1, B2>::count == 0, "");
    static_assert( variadic_counter<tag, B1, B2, tag>::count == 1, "");
    static_assert( variadic_counter<tag, B1, B2, tag, other_tag>::count == 1, "");

    static_assert( variadic_counter<other_tag, A2>::count == 0, "");
    static_assert( variadic_counter<other_tag, A1, A2>::count == 0, "");
    static_assert( variadic_counter<other_tag, B1, A2>::count == 1, "");
    static_assert( variadic_counter<other_tag, B1>::count == 1, "");
    static_assert( variadic_counter<other_tag, B1, B2>::count == 2, "");
    static_assert( variadic_counter<other_tag, B1, B2, tag>::count == 2, "");
    static_assert( variadic_counter<other_tag, B1, B2, tag, other_tag>::count == 3, "");

    return 0;
}
