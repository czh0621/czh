/**
 * @file       when.h
 * @date       2023/9/27
 * @since      2023-09-27
 * @author     czh
 */

#ifndef CZH_WHEN_H_
#define CZH_WHEN_H_
#include "../core_util/function_traits.h"
#include "future.h"

namespace czh {
namespace when_internal {
    template<typename... Future>
    class WhenAllContext : public std::enable_shared_from_this<WhenAllContext<Future...>>
    {
    public:
        template<typename F, typename Idx>
        void operator()(F&& fut, Idx i)
        {
            using value_type = typename czh::TryWrapper<typename std::decay_t<F>::InnerType>::type;
            auto self        = this->shared_from_this();
            fut.then([self](value_type&& value) {
                std::unique_lock<std::mutex> lck(self->m_mutex);
                self->m_count++;
                // 此处不能 使用 std::get<Idx>(m_tuple) = std::move(value); 因为 Idx 为
                // integral_constant type std::get<Idx>(m_tuple) = std::move(value);
                std::get<decltype(i)::value>(self->m_tuple) = std::move(value);
                if (sizeof...(Future) == self->m_count) {
                    lck.unlock();
                    self->m_tuple_prom.set_value(std::move(self->m_tuple));
                }
            });
        }

        template<typename Tuple>
        void for_each(Tuple&& t)
        {
            for_each_tp(*this, t, std::index_sequence_for<Future...>{});
        }

        czh::Future<typename std::tuple<
            typename czh::TryWrapper<typename std::decay_t<Future>::InnerType>::type...>>
        get_future()
        {
            return m_tuple_prom.get_future();
        }



    private:
        czh::Promise<
            std::tuple<typename czh::TryWrapper<typename std::decay_t<Future>::InnerType>::type...>>
            m_tuple_prom;
        std::tuple<typename czh::TryWrapper<typename std::decay_t<Future>::InnerType>::type...>
                   m_tuple;
        std::mutex m_mutex;
        size_t     m_count{0};
    };
}   // namespace when_internal

template<typename... T>
decltype(auto) when_all(T&&... future)
{
    auto ctx = std::make_shared<when_internal::WhenAllContext<T...>>();
    ctx->for_each(std::forward_as_tuple(std::forward<T>(future)...));
    return ctx->get_future();
}

template<typename Iterator>
using iterator_value_type =
    typename TryWrapper<typename std::iterator_traits<Iterator>::value_type::InnerType>::type;


template<typename Iterator>
decltype(auto) when_all(Iterator first, Iterator end)
{
    if (first == end) {
        return make_ready_future(std::vector<iterator_value_type<Iterator>>());
    }
    struct AllContext
    {
        explicit AllContext(size_t n)
            : v(n){};
        std::vector<iterator_value_type<Iterator>> v;
        Promise<iterator_value_type<Iterator>>     prom;
        std::atomic<size_t>                        collected{0};
    };
    auto ctx = std::make_shared<AllContext>(std::distance(first, end));
    for (size_t i = 0; first != end; ++first, ++i) {
        first->then([ctx, i](iterator_value_type<Iterator>&& t) {
            ctx->v[i] = std::move(t);
            // atomic_fetch_add 原子变量+i 返回之前的value
            if (ctx->v.size() - 1 == std::atomic_fetch_add(&ctx->collected, std::size_t(1))) {
                ctx->prom.set_value(std::move(ctx->v));
            }
        });
    }
    return ctx->prom.get_future();
}

template<typename Iterator>
decltype(auto) when_any(Iterator first, Iterator end)
{
    if (first == end) {
        return make_ready_future(std::pair<size_t, iterator_value_type<Iterator>>(
            size_t(0), iterator_value_type<Iterator>()));
    }
    struct AnyContext
    {
        AnyContext() = default;
        Promise<std::pair<size_t, iterator_value_type<Iterator>>> prom;
        std::atomic<bool>                                         done{false};
    };
    auto ctx = std::make_shared<AnyContext>();
    for (size_t i = 0; first != end; ++first, ++i) {
        first->then([ctx, i](iterator_value_type<Iterator>&& t) {
            if (!ctx->done.exchange(true)) {
                ctx->prom.set_value(std::make_pair(i, std::move(t)));
            }
        });
    }
    return ctx->prom.get_future();
}

template<typename Iterator>
decltype(auto) when_n(size_t n, Iterator first, Iterator end)
{
    size_t n_futures    = std::distance(first, end);
    auto   need_collect = std::min<size_t>(n, n_futures);
    if (need_collect == 0) {
        return make_ready_future(std::vector<std::pair<size_t, iterator_value_type<Iterator>>>(
            std::make_pair(0, iterator_value_type<Iterator>())));
    }
    struct NContext
    {
        explicit NContext(size_t n)
            : needs(n){};
        std::vector<std::pair<size_t, iterator_value_type<Iterator>>>          v;
        Promise<std::vector<std::pair<size_t, iterator_value_type<Iterator>>>> prom;
        std::mutex                                                             mutex;
        const size_t                                                           needs;
        bool                                                                   done{false};
    };
    auto ctx = std::make_shared<NContext>(need_collect);
    for (size_t i = 0; first != end; ++first, ++i) {
        first->then([ctx, i](iterator_value_type<Iterator>&& t) {
            std::unique_lock<std::mutex> lck(ctx->mutex);
            if (ctx->done) {
                return;
            }
            ctx->v.emplace(std::make_pair(i, std::move(t)));

            if (ctx->v.size() == ctx->needs) {
                ctx->done = true;
                lck.unlock();
                ctx->prom.set_value(std::move(ctx->v));
            }
        });
    }
    return ctx->prom.get_future();
}


}   // namespace czh








#endif   // CZH_WHEN_H_
