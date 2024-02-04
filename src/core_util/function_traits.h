/**
 * @file       function_traits.h
 * @date       2023/9/15
 * @since      2023-09-15
 * @author     czh
 */

#ifndef CZH_FUNCTION_TRAITS_H_
#define CZH_FUNCTION_TRAITS_H_
#include <functional>
#include <initializer_list>
#include <tuple>
#include <type_traits>

// https://github.com/rocwangp/code_snippets/blob/master/funciton_traits.cc


template<typename F>
struct FunctionTraits;

template<typename Ret, typename... Args>
struct FunctionTraits<Ret(Args...)>
{
    // 如果 Args 中包含 void，这并不会导致问题。void 会被视为一个有效的类型，但它不会影响
    // sizeof...(Args) 的计算，因为它只计算非 void 类型的数量
    static constexpr auto size = sizeof...(Args);
    using return_type          = Ret;
    using tuple_type           = std::tuple<Args...>;
    using bare_tuple_type      = std::tuple<std::decay_t<Args>...>;
};

template<typename Ret>
struct FunctionTraits<Ret()>
{
    // 如果 Args 中包含 void，这并不会导致问题。void 会被视为一个有效的类型，但它不会影响
    // sizeof...(Args) 的计算，因为它只计算非 void 类型的数量
    static constexpr auto size = 0;
    using return_type          = Ret;
    // std::tuple<> 为空元组  std::tuple<void> 为存储void的元组（void
    //     没有实际的值）且std::tuple<void> 无法实例化
    using tuple_type      = std::tuple<>;
    using bare_tuple_type = std::tuple<>;
};

template<typename Ret, typename... Args>
struct FunctionTraits<Ret (*)(Args...)> : public FunctionTraits<Ret(Args...)>
{};

template<typename Ret, typename... Args>
struct FunctionTraits<std::function<Ret(Args...)>> : public FunctionTraits<Ret(Args...)>
{};

#define FUNCTION_TRAITS(...)                                       \
    template<typename Ret, typename ClassType, typename... Args>   \
    struct FunctionTraits<Ret (ClassType::*)(Args...) __VA_ARGS__> \
        : public FunctionTraits<Ret(Args...)>                      \
    {                                                              \
        using class_type = ClassType;                              \
    };
FUNCTION_TRAITS();
FUNCTION_TRAITS(const);
FUNCTION_TRAITS(volatile);
FUNCTION_TRAITS(const volatile);

//// 对于成员函数 const 与非const函数特化
// template<typename Ret, typename ClassType, typename... Args>
// struct FunctionTraits<Ret (ClassType::*)(Args...) const> : public FunctionTraits<Ret(Args...)>
//{
//     using class_type = ClassType;
// };

template<typename Callable>
struct FunctionTraits : public FunctionTraits<decltype(&Callable::operator())>
{};


////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename F, typename... Args, std::size_t... Idx>
void for_each_tp(F&& f, const std::tuple<Args...>& tp, std::index_sequence<Idx...>)
{
    (void)std::initializer_list<int>{
        (std::forward<F>(f)(std::get<Idx>(tp), std::integral_constant<int, Idx>{}), 0)...};
}



////////////////////////////////////////////////////////////////////////////////////////////////////






#endif   // CZH_FUNCTION_TRAITS_H_
