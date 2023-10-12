/**
 * @file       TestPtr_smart_ptr.cpp
 * @date       2023/9/12
 * @since      2023-09-12
 * @author     czh
 */
#include "smart_ptr.h"
#include <map>

// 测试enable_shared_from_this
void test_return_ptr()
{
    auto* t_ptr = new TestPtr();
    // smart_ptr1 和 smart_ptr2  t_ptr 三个指针同时管理同一个对象，double free 且
    // 两个智能指针的引用计数相互独立。
    auto smart_ptr1 = t_ptr->get_ptr();
    auto smart_ptr2 = t_ptr->get_ptr();
    spdlog::info(
        "smart_ptr1 count:{} type:{}", smart_ptr1.use_count(), typeid(smart_ptr1.get()).name());
    spdlog::info(
        "smart_ptr2 count:{} type:{}", smart_ptr2.use_count(), typeid(smart_ptr2.get()).name());
    spdlog::info("t_ptr addr:{} smart_ptr addr:{}", (int64_t)t_ptr, (int64_t)smart_ptr1.get());
    // double free 了 ，原因手动管理t_ptr 和智能指针管理 同一个对象 使用asan检查
    //    {
    //        delete t_ptr;
    //        t_ptr = nullptr;
    //    }

    spdlog::info("TestPtr str:{}", smart_ptr1->m_str);
    std::shared_ptr<TestPtr> spt(new TestPtr());
    spdlog::info("当前资源被引用数:{}", spt.use_count());   // 1
    std::shared_ptr<TestPtr> sptb = spt->get_shared_ptr();
    spdlog::info("当前资源被引用数:{}", sptb.use_count());   // 2
}
// 测试继承关系下的enable_shared_from_this
void test_base_ptr()
{
    auto                  derived_ptr = std::make_shared<Derived>();
    std::shared_ptr<Base> base_ptr    = derived_ptr;   // 隐式转换 向上转换安全
    auto                  ptr         = derived_ptr->shared_from_this();

    // 实际上 三个指针都指向Derived这个对象
    spdlog::info("derived_ptr count:{} type:{}",
                 derived_ptr.use_count(),
                 typeid(derived_ptr.get()).name());   // Derived type
    spdlog::info("base_ptr count:{} type:{}",
                 base_ptr.use_count(),
                 typeid(base_ptr.get()).name());                                       // Base type
    spdlog::info("ptr count:{} type:{}", ptr.use_count(), typeid(ptr.get()).name());   // Base type

    ptr->call();   // base ptr  can ptr->m_base; can not ptr->m_derived 虚函数

    std::shared_ptr<Derived> convert_ptr  = std::dynamic_pointer_cast<Derived>(ptr);
    auto                     convert_ptr2 = std::static_pointer_cast<Derived>(ptr);
    convert_ptr->call();   // Derived ptr  can ptr->m_base; also can ptr->m_derived
    convert_ptr2->call();
}

class BaseA
{
    std::string m_str{"A"};
};

class DerivedB : public BaseA
{
    std::string m_str{"B"};
};


void test_convert_ptr()
{
    auto                   d_ptr              = std::make_shared<DerivedB>();
    std::shared_ptr<BaseA> b_ptr              = d_ptr;
    auto                   static_convert_ptr = std::static_pointer_cast<DerivedB>(b_ptr);
    // error In template: 'BaseA' is not polymorphic 必须是多态类型才可以转换
    //    auto                   dynamic_convert_ptr = std::dynamic_pointer_cast<DerivedB>(b_ptr);
}

void test_unique_ptr()
{
    std::unique_ptr<int>                ptr = std::make_unique<int>(5);
    std::map<int, std::unique_ptr<int>> test_map;
    // copy delete only move
    // test_map[5] = ptr;
    test_map.emplace(5, std::move(ptr));
    test_map[5] = std::move(ptr);

    // only move
    // auto u_ptr  = test_map[5];
    auto u_ptr2 = std::move(test_map[5]);   // map 中5号索引存储的智能指针被掏空，置为默认值NULL
    spdlog::info("map size:{}", test_map.size());   // print 1
}




int main()
{
    //    test_return_ptr();
    //    test_base_ptr();
    //    test_convert_ptr();
    test_unique_ptr();
    return 0;
}