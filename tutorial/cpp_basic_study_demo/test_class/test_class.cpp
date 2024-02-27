/**
 * @file       test_class.cpp
 * @date       2023/9/12
 * @since      2023-09-12
 * @author     czh
 */
#include "class.h"
void test_class_memory()
{
    spdlog::info("empty class memory size:{}", sizeof(EmptyClass));
    spdlog::info("virtual class memory size:{}", sizeof(VirtualClass));
    spdlog::info("common class memory size:{}", sizeof(CommonClass));
    spdlog::info("VirtualClassAlignas class memory size:{}", sizeof(VirtualClassAlignas));
}
void test_class_func()
{
    Derived d;
    Base    b;
    Base*   ptr = &d;
    spdlog::info("Derived address:{}", (int64_t)&d);
    spdlog::info("Base size:{} Derived size:{}", sizeof(Base), sizeof(Derived));
    auto p = (char*)&d + 8;
    int* t = reinterpret_cast<int*>(p);
    spdlog::info("Derived +8 {}", *t);   // 虚函数的地址
    int offset = (char*)&d - (char*)ptr;
    spdlog::info("Offset {}", offset);
    spdlog::info("test ptr->Base ---------------------");
    ptr = &b;
    ptr->print();
    ptr->common();
    ptr->f1();
    // ptr->f2();//编译不过 ‘class Base’ has no member named ‘f2’
    ptr->override();
    ptr->redefine();

    spdlog::info("test ptr->derived ---------------------");
    ptr = &d;
    ptr->print();
    ptr->common();
    ptr->f1();
    // ptr->f2();//编译不过 ‘class Base’ has no member named ‘f2’
    ptr->override();
    ptr->redefine();
    auto ptr_derived = dynamic_cast<Derived*>(ptr);
    ptr_derived->f2();
    ptr_derived->redefine();
    ptr_derived->redefine(1);
}

void test_multi_public()
{
    Derived2 d;
    BaseA*   pA = &d;
    BaseB*   pB = &d;
    {
        // 基类转子类，此处不涉及多态
        Derived2* pD = dynamic_cast<Derived2*>(pA);
        if (pD != nullptr) {
            spdlog::info("pD ptr is not nullptr");
            pD->funcA();
            pD->funcB();
        }
        pD = (Derived2*)pA;
        pD->funcA();
        pD->funcB();
        // 都是子类地址 调用子类函数
        // 此时也走虚函数表（读取的时子类的虚函数表），只不过这里没有重写虚函数，调用的都是基类函数。
        d.common1();
        d.common2();
    }
    spdlog::info("----------------------");
    {
        // 基类2转基类1，此处设计多态。
        BaseA* pA1 =
            dynamic_cast<BaseA*>(pB);   // 此处dynamic_cast会调整vptr，将指向B类的vptr改为指向A类
        if (pA1 != nullptr) {
            spdlog::info("pA1 ptr is not nullptr");   // 调用A虚函数表的虚函数
            pA1->funcA();                             // funcA 被重写
            pA1->common1();                           // 没被重写
        }
        pA1 = (BaseA*)pB;   // 此处不会调整vptr的指向 仍然指向B表
        pA1->funcA();   // 此处语义上是调用funcA 但是由于vptr指向B表 回调用B表的中虚函数
        pA1->common1();   // 同上
    }
    spdlog::info("----------------------");
    {
        BaseB* pB2 = dynamic_cast<BaseB*>(pA);   // 进行了地址偏移，pB2地址指向了BaseB部分地址
        if (pB2 != nullptr) {
            spdlog::info("dynamic_cast addr :{}", (int64_t)pB2);   // 140724103650080
            pB2->funcB();
            // pB2->common2(); 编译不过！
        }
        pB2 = (BaseB*)
            pA;   // PA
                  // 原来指向的就是BaseA部分，进行强转过后，未进行偏移，此时指向的还是子类对象首地址
        spdlog::info("普通转换 addr :{}", (int64_t)pB2);   // 调用A虚函数表的虚函数 140724103650064

        // 因此调用的都是BaseA内的虚函数表中的虚函数
        pB2->funcB();     // Derived2::funcA()
        pB2->common2();   // BaseA::common1()

        auto ptr = &d;
        spdlog::info("ptr addr :{}", (int64_t)ptr);   // 调用A虚函数表的虚函数
        ptr->funcB();
        ptr->common2();
    }
}

void test_class_constructor()
{
    Test_Base* ptr = new Test_Derived(1);

    ptr->T_func1();

    auto d = Test_Derived(2);
    d.T_func1();
    //    auto b = Test_Base(2);
    //    b.T_func1();
}


void test_class_special_mem_func()
{
    MyClass obj1;                    // 调用默认构造函数
    MyClass obj2(obj1);              // 调用拷贝构造函数
    MyClass obj3(std::move(obj1));   // 调用移动构造函数
    MyClass obj4;
    obj4 = obj2;   // 调用拷贝赋值操作符
    MyClass obj5;
    obj5         = std::move(obj2);   // 调用移动赋值操作符
    MyClass obj6 = obj1;              // 调用拷贝构造函数
    MyClass obj7;                     // 调用默认构造函数
    MyClass obj8 = std::move(obj7);   // 调用移动构造函数
}



int main()
{
    //    test_class_memory();
    //    test_class_func();

    //    test_multi_public();

    //    test_class_constructor();

    test_class_special_mem_func();
    return 0;
}