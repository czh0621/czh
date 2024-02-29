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

    // 多继承指针转换
    std::shared_ptr<Base2> base2_ptr = derived_ptr;   // 隐式转换 向上转换安全
    base2_ptr->call_base2_func();
    base2_ptr->call();   // derived call 虚函数

    // Base -->Base2
    auto convert_base2_ptr = std::dynamic_pointer_cast<Base2>(base_ptr);
    convert_base2_ptr->call_base2_func();
    // convert_base2_ptr->call_base_func(); // error Base2 no func call_base_func
    convert_base2_ptr->call();   // derived call 虚函数表重写了

    // Base2 -->Base
    auto convert_base_ptr = std::dynamic_pointer_cast<Base>(base2_ptr);
    // convert_base_ptr->call_base2_func(); // error Base2 no func call_base_func
    convert_base_ptr->call_base_func();
    convert_base2_ptr->call();   // derived call  虚函数表重写了
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
    std::unique_ptr<int>                ptr5 = std::make_unique<int>(5);
    std::unique_ptr<int>                ptr6(new int(6));
    std::map<int, std::unique_ptr<int>> test_map;
    // copy delete only move
    // test_map[5] = ptr;
    test_map.emplace(5, std::move(ptr5));

    test_map[6] = std::move(ptr6);

    // only move
    // auto u_ptr  = test_map[5];
    auto u_ptr_move5 =
        std::move(test_map[5]);   // map 中5号索引存储的智能指针被掏空，置为默认值NULL
    spdlog::info("map size:{} ", test_map.size());   // print 2

    // 断言为真 不打印！false 触发
    assert(test_map[5] == nullptr &&
           "test_map[5] has been moved, now test_map[5] should be nullptr,but not!");

    // release 只是放弃控制权,返回裸指针,将智能指针置为nullptr
    auto release_ret = u_ptr_move5.release();
    bool value       = (u_ptr_move5 == nullptr);
    spdlog::info("u_ptr_move5.release() return raw pointer:{} (u_ptr_move5 ==nullptr)value:{}",
                 (void*)release_ret,
                 value);   // print true

    auto delete_wrapper = [](int* p) {
        if (p) {
            spdlog::info("call delete this object");
            delete p;
        }
    };

    // 自定义删除器 写在模板中
    std::unique_ptr<int, decltype(delete_wrapper)> ptr(new int(1), delete_wrapper);

    // 释放原来指向的对象，并指向新的对象
    ptr.reset(test_map[6].release());

    // 释放指向的对象,并设置内部对象指针为nullptr
    ptr.reset();

    assert(ptr == nullptr && "ptr should not nullptr");

    // 直接转换shared_ptr
    std::shared_ptr<int> p = std::make_unique<int>(0);
}



void test_shared_ptr()
{
    // std::shared_ptr<int> ptr0 = new int();   // error
    std::shared_ptr<int> ptr1(new int(1));
    std::shared_ptr<int> ptr2 = std::make_shared<int>(2);

    auto delete_wrapper = [](int* p) {
        if (p) {
            spdlog::info("delete this object param p");
            delete p;
        }
    };
    std::shared_ptr<int> ptr3(new int(1), delete_wrapper);

    ptr3 = ptr2;   // ptr3 指向 ptr2 object 即ptr3原来的memory 计数为0 call delete_wrapper

    spdlog::info("ptr3 use_count:{} should be 2", ptr3.use_count());

    auto delete_wrapper2 = [](int* ptr) {
        if (ptr) {
            spdlog::info("delete this object param ptr");
            delete ptr;
        }
    };

    // std::shared_ptr<int> ptr4(ptr_copy, delete_wrapper2); //error 参数应该为裸指针

    std::shared_ptr<int> ptr5(ptr1);   // ptr5和ptr1 都指向同一对象 增加计数
    spdlog::info("ptr5 use_count:{} should be 2", ptr5.use_count());

    ptr3.reset();   // 减少计数
    assert(ptr3 == nullptr && "ptr3 should not nullptr");
    ptr5.reset(new int(5), delete_wrapper2);   // 减少原来的计数，增加指向新的memory计数

    std::shared_ptr<int> ptr_tmp = std::make_shared<int>();
    auto                 ptr_tmp2(ptr_tmp);

    // reset 同一个裸指针 同样导致内存二次释放

    //    ptr5.reset(ptr_tmp.get());
    //    spdlog::info("ptr5 reset ptr_tmp use_count:{} ptr_tmp2 use_count:{}",
    //                 ptr5.use_count(),
    //                 ptr_tmp.use_count()); // print 1 2 同样导致内存二次释放
    ptr5 = nullptr;   // 清空ptr5的引用计数,对象指针为nullptr


    {
        auto*                        p = new std::string("hello");
        std::shared_ptr<std::string> sp1(p);
        /*不要这样做！！*/
        std::shared_ptr<std::string> sp2(p);   // error 二次释放
    }
    {
        auto         sp = std::make_shared<std::string>("wechat:shouwangxiansheng");
        std::string* p  = sp.get();
        {
            // error
            // std::shared_ptr<std::string> sp2(p);/*不要这样做!!*/
        }
        // error
        // delete p; /*不要这样做*/
    }
}




int main()
{
    //    test_return_ptr();

    test_base_ptr();


    //    test_convert_ptr();

    // test_unique_ptr();

    //    test_shared_ptr();
    return 0;
}