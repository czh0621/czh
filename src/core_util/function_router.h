/**
 * @file       function_router.h
 * @date       2023/9/15
 * @since      2023-09-15
 * @author     czh
 */

#ifndef CZH_FUNCTION_ROUTER_H_
#define CZH_FUNCTION_ROUTER_H_
#include "function_traits.h"
#include <iostream>
#include <string>
#include <unordered_map>
// 函数反射
namespace czh { namespace core {
    class Router
    {
    public:
        decltype(auto) get_iterator(const std::string& method_name)
        {
            auto it = m_router_map.find(method_name);
            return it;
        }


        // 此处需注意注册的函数与invoke/call 调用时的参数类型必须请强匹配！！！
        template<typename F,
                 typename = std::enable_if_t<!std::is_member_function_pointer<F>::value>>
        bool register_func_handler(const std::string& method_name, F&& f)
        {
            if (check_duplicate(method_name)) {
                return false;
            }
            m_router_map.emplace(
                method_name,
                std::bind(&invoker<F>::apply, f, std::placeholders::_1, std::placeholders::_2));
            //            m_router_map[method_name] = {
            //                std::bind(&invoker<F>::apply, f, std::placeholders::_1,
            //                std::placeholders::_2)};
            return true;
        }

        template<typename F, typename Self,
                 typename = std::enable_if_t<std::is_member_function_pointer<F>::value>>
        bool register_func_handler(const std::string& method_name, Self* self, F&& f)
        {
            if (check_duplicate(method_name)) {
                return false;
            }
            m_router_map.emplace(method_name,
                                 std::bind(&invoker<F>::template apply_member<Self>,
                                           f,
                                           self,
                                           std::placeholders::_1,
                                           std::placeholders::_2));
            //            m_router_map[method_name] = {
            //                std::bind(&invoker<F>::apply, f, std::placeholders::_1,
            //                std::placeholders::_2)};
            return true;
        }

        // 此处在写多个参数 包含可变参数包时 ，可变参数包放在对后一个，防止编译器无法识别的问题
        template<typename T, typename... Args>
        bool invoke_method(T& value, const std::string& method_name, Args&&... args)
        {
            return call_method_impl(method_name, &value, std::forward<Args>(args)...);
        }

        template<typename... Args>
        bool call_method(const std::string& method_name, Args&&... args)
        {
            return call_method_impl(method_name, nullptr, std::forward<Args>(args)...);
        }

    private:
        template<typename... Args>
        bool call_method_impl(const std::string& method_name, void* result, Args&&... args)
        {
            auto it = get_iterator(method_name);
            if (it == m_router_map.end()) {
                return false;
            }
            //            auto param_tuple = std::forward_as_tuple<Args...>(args...);

            //  std::tuple<Args...>(args) 会将整个args对象作为作为一个参数传给tuple
            //  而std::tuple<Args...>(args...) 会将参数展开列表 匹配模板参数包

            // 若不退化则参数构造函数有问题,std::tuple<int, double&>::tuple()
            //            auto param_tuple = std::tuple<Args...>(args...);
            //            与下面的区别就是是否退化（查看std::make_tuple实现）
            auto param_tuple = std::make_tuple(std::forward<Args>(args)...);
            using tuple_type = decltype(param_tuple);
            std::cout << "----," << typeid(tuple_type).name() << "," << std::endl;
            using storage_tuple =
                typename std::aligned_storage<sizeof(tuple_type), alignof(tuple_type)>::type;
            storage_tuple tuple_data;
            auto*         tp_params = new (&tuple_data) tuple_type;
            *tp_params              = param_tuple;
            it->second(result, tp_params);
            tp_params->~tuple_type();
            return true;
        }

    private:
        bool check_duplicate(const std::string& method_name)
        {
            auto it = get_iterator(method_name);

            return it == m_router_map.end() ? false : true;
        }

        template<typename Fn>
        struct invoker
        {

            // wrapper 统一调用形式
            static inline void apply(const Fn& f, void* result, void* param)
            {
                using tuple_type     = typename FunctionTraits<Fn>::bare_tuple_type;
                const tuple_type* tp = static_cast<tuple_type*>(param);
                call(f, *tp, result);
            }

            template<typename Self>
            static inline void apply_member(const Fn& f, Self* self, void* result, void* param)
            {
                using tuple_type     = typename FunctionTraits<Fn>::bare_tuple_type;
                const tuple_type* tp = static_cast<tuple_type*>(param);

                using return_type = typename FunctionTraits<Fn>::return_type;
                // std::integral_constant<bool, std::is_void<return_type>::value>{}
                //  using true_type =  integral_constant<bool, true>;
                //
                //  The type used as a compile-time boolean with false value.
                //  using false_type = integral_constant<bool, false>;
                // 可以构造编译期常量
                call_member(f,
                            self,
                            *tp,
                            result,
                            std::integral_constant<bool, std::is_void<return_type>::value>{});
            }

            template<typename F, typename... Args>
            static auto call(F&& f, const std::tuple<Args...>& tp, void* result) ->
                typename std::enable_if_t<!std::is_void<std::result_of_t<F(Args...)>>::value,
                                          std::result_of_t<F(Args...)>>
            {
                auto ret = call_helper(f, std::make_index_sequence<sizeof...(Args)>{}, tp);
                if (result) {
                    *(decltype(ret)*)result = ret;
                }
                return ret;
            }

            template<typename F, typename... Args>
            static auto call(F&& f, const std::tuple<Args...>& tp, void* result) ->
                typename std::enable_if_t<std::is_void<std::result_of_t<F(Args...)>>::value,
                                          std::result_of_t<F(Args...)>>
            {
                return call_helper(f, std::make_index_sequence<sizeof...(Args)>{}, tp);
            }

            // 举个例子，假设 N... 是一个包含 {1, 2, 3} 的序列，同时 tp 是一个包含三个元素的元组 {a,
            // b, c}：
            //
            // std::get<N...>(tp) 将会展开为 std::get<1>(tp), std::get<2>(tp),
            // std::get<3>(tp)，返回一个包含 b, c, d 的新元组。
            // std::get<N>(tp)... 将会展开为 std::get<1>(tp), std::get<2>(tp),
            // std::get<3>(tp)，这将会在编译时展开为三个函数调用，分别返回 b, c, d。
            template<typename F, typename... Args, size_t... N>
            static auto call_helper(F&&                        f, const std::index_sequence<N...>&,
                                    const std::tuple<Args...>& tp) -> std::result_of_t<F(Args...)>
            {
                // std::get<N...>(tp)
                // std::get<N>(tp)...
                return f(std::get<N>(tp)...);
            }
            template<typename F, typename Self, typename... Args>
            static auto call_member(F&& f, Self* self, const std::tuple<Args...>& tp, void* result,
                                    std::true_type)
            {
                return call_member_helper(f, self, std::make_index_sequence<sizeof...(Args)>{}, tp);
            }

            template<typename F, typename Self, typename... Args>
            static auto call_member(F&& f, Self* self, const std::tuple<Args...>& tp, void* result,
                                    std::false_type)
            {
                auto ret =
                    call_member_helper(f, self, std::make_index_sequence<sizeof...(Args)>{}, tp);
                if (result) {
                    *(decltype(ret)*)result = ret;
                }
                return ret;
            }

            template<typename F, typename Self, typename... Args, size_t... N>
            static auto call_member_helper(F&& f, Self* self, const std::index_sequence<N...>&,
                                           const std::tuple<Args...>& tp)
                -> decltype((self->*f)(std::get<N>(tp)...))
            {
                return (self->*f)(std::get<N>(tp)...);
            }
        };

    private:
        // 函数原型统一
        std::unordered_map<std::string, std::function<void(void*, void*)>> m_router_map;
    };

}}   // namespace czh::core

#endif   // CZH_FUNCTION_ROUTER_H_
