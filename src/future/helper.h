/**
 * @file       helper.h
 * @date       2023/9/11
 * @since      2023-09-11
 * @author     czh
 */

#ifndef CZH_HELPER_H_
#define CZH_HELPER_H_
#include <type_traits>
namespace czh {
template<typename T>
class Future;

template<typename T>
class Try;

template<typename F, typename... Args>
using return_type = typename std::result_of_t<F(Args...)>;

//    template<typename F, typename... Args>
//    using return_type = decltype(std::declval<F>()(std::declval<Args>()...));

template<typename F, typename... Args>
struct ResultOfWrapper
{
    using type = return_type<F, Args...>;
};

template<typename F, typename... Args>   // 此处的F为Invokable类的模板参数
struct Invokable
{
    // 此处又在实例化模板函数,Func与Invokable类的模板参数不能相同！相同报错！declaration of template
    // parameter ‘F’ shadows template parameter
    template<typename Func, typename Dummy = return_type<Func, Args...>>
    constexpr static std::true_type check(std::nullptr_t dummy)
    {
        return std::true_type{};
    }

    template<typename Func>
    constexpr static std::false_type check(...)
    {
        return std::false_type{};
    }

    // 编译器优先匹配参数为std::nullptr_t的函数
    typedef decltype(check<F>(nullptr)) type;
    constexpr static bool               value = type::value;
};

template<typename T>
struct IsFuture : public std::false_type
{
    using inner_type = T;
};

template<typename T>
struct IsFuture<Future<T>> : public std::true_type
{
    using inner_type = T;
};

template<typename F, typename T>
struct CallableResult
{
    // std::conditional<bool, typename1 ,typename2> typename1此处填写return_type<F> 时
    // 由于Invokable<F>实例化，会导致return_type<F>没有type 报错！no type named ‘type’ in ‘class
    // 因此迎合编译器需要在包一层type

    // typedef typename std::conditional<Invokable<F>::value, return_type<F>, typename
    // std::conditional<Invokable<F, T &&>::value, return_type<F, T &&>, return_type<F, T
    // &>>::type>::type Return_type;

    using Return_type = typename std::conditional<
        Invokable<F>::value, ResultOfWrapper<F>,
        typename std::conditional<Invokable<F, T&&>::value, ResultOfWrapper<F, T&&>,
                                  ResultOfWrapper<F, T&>>::type>::type;
    typedef IsFuture<typename Return_type::type>            IsFutureReturnType;
    typedef Future<typename IsFutureReturnType::inner_type> ReturnFutureType;
};


template<typename F>
struct CallableResult<F, void>
{
    using Return_type = typename std::conditional<
        Invokable<F>::value, ResultOfWrapper<F>,
        typename std::conditional<Invokable<F, Try<void>&&>::value, ResultOfWrapper<F, Try<void>&&>,
                                  ResultOfWrapper<F, Try<void>&>>::type>::type;
    using IsFutureReturnType = IsFuture<typename Return_type::type>;
    using ReturnFutureType   = Future<typename IsFutureReturnType::inner_type>;
};


};   // namespace czh

#endif   // CZH_HELPER_H_
