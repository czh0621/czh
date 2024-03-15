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
    spdlog::info("virtual class memory size:{} int* size:{}", sizeof(VirtualClass), sizeof(int*));
    spdlog::info("common class memory size:{}", sizeof(CommonClass));
    spdlog::info("VirtualClassAlignas class memory size:{}", sizeof(VirtualClassAlignas));
}

void test_multi_public()
{
    Derived2 d;
    BaseA*   pA = &d;
    BaseB*   pB = &d;
    spdlog::info("BaseA*(pointer to Derived2)--->Derived2*----------------------");
    {
        // 基类转子类，此处不涉及多态
        Derived2* pD = dynamic_cast<Derived2*>(pA);
        if (pD != nullptr) {
            spdlog::info("pD ptr is not nullptr ");
            // 以下都是子类common调用
            pD->funcA();
            pD->funcB();
            pD->common_func();
            pD->func();
            pD->commonA();
            pD->commonB();
        }
        spdlog::info("----------------------");
        {
            // 以下都是子类common调用
            auto* tmp = (Derived2*)pA;
            tmp->funcA();
            tmp->funcB();
            tmp->common_func();
            tmp->func();
            tmp->commonA();
            tmp->commonB();
        }
        spdlog::info("----------------------");
        {
            // 以下都是子类common调用
            auto* tmp = (Derived2*)pB;
            tmp->funcA();
            tmp->funcB();
            tmp->common_func();
            tmp->func();
            tmp->commonA();
            tmp->commonB();
        }
    }
    spdlog::info("BaseB*(pointer to Derived2)--->BaseA*----------------------");
    {
        // 基类2转基类1，指针指向的还是子类对象 所以用的还是子类的虚函数表
        BaseA* pA1 =
            dynamic_cast<BaseA*>(pB);   // 此处dynamic_cast会调整vptr，将指向B类的vptr改为指向A类
        if (pA1 != nullptr) {
            spdlog::info("pA1 ptr is not nullptr");   // 调用A虚函数表的虚函数
            pA1->funcA();                             // Derived2
            // pA1->funcB(); error  A类 no funcB
            pA1->common_func();
            pA1->func();
            pA1->commonA();
            // pA1->commonB() error 同上
        }
        spdlog::info("----------------------");
        {
            pA1 = (BaseA*)pB;   // 此处不会调整vptr的指向 仍然指向B表
            pA1->funcA();       // error call 居然 call override Derived2 funcB
            // pA1->funcB(); error  A类 no funcB
            pA1->common_func();
            pA1->func();
            pA1->commonA();   // error call  call virtual BaseB commonB
            // pA1->commonB() error 同上
        }
    }
    // 重点是这种情况！由于上述转换的指针恰好指向的基类的第一个类，不需要VPTR指针移动
    spdlog::info("BaseA*(pointer to Derived2)--->BaseB*----------------------");
    {
        BaseB* pB2 = dynamic_cast<BaseB*>(pA);   // 进行了地址偏移，pB2地址指向了BaseB部分地址
        if (pB2 != nullptr) {
            spdlog::info("dynamic_cast addr :{}", (int64_t)pB2);   // 140724103650080
            // pB2->funcA();
            pB2->funcB();
            // pB2->common_func();
            // pB2->func();
            // pB2->commonA();
            pB2->commonB();
        }
        spdlog::info("----------------------");
        {
            pB2 = (BaseB*)
                pA;   // PA
                      // 原来指向的就是BaseA部分，进行强转过后，未进行偏移，此时指向的还是子类对象首地址
            spdlog::info("普通转换 addr :{}",
                         (int64_t)pB2);   // 调用A虚函数表的虚函数 140724103650064

            // pB2->funcA();
            pB2->funcB();   // 此处实际调用的是A类部分的虚函数表中的第一个函数
            // pB2->common_func();
            // pB2->func();
            // pB2->commonA();
            pB2->commonB();
        }
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
    obj5         = std::move(obj2);                           // 调用移动赋值操作符
    MyClass obj6 = obj1;                                      // 调用拷贝构造函数
    MyClass obj7;                                             // 调用默认构造函数
    MyClass obj8 = std::move(obj7);                           // 调用移动构造函数
    spdlog::info("obj7 member_value:{}", obj7.get_value());   // empty string
}


void test_virtual_class()
{
    {
        // 单继承调用
        ClassA* a = new ClassB();
        a->func1();   // "ClassA::func1()"   普通函数调用 指针类型决定
        // 若A类注释掉void func1()
        // a->func1();                  //class ClassA’ has no member named ‘func1‘
        // 说明A类型的指针a只能操作属于基类的内存范围
        a->func2();    // "ClassA::func2()"
        a->vfunc1();   // "ClassB::vfunc1()"  B类的虚函数表重写了A类的vfunc1()
        a->vfunc2();   // "ClassA::vfunc2()"  B类的虚函数表 未重写直接继承A类的虚函数
    }



    // 多次单继承调用
    ClassA* a = new ClassC;
    ////////因为是A类指针 不是虚函数 普通调用只访问A类的函数
    a->func1();   // "ClassA::func1()"
    a->func2();   // "ClassA::func2()"

    a->vfunc1();   // "ClassB::vfunc1()"	C类的虚函数表未重写
                   // ，这里继承的B类的虚函数表(但是B类的虚函数表重写了A类的vfunc1())
    a->vfunc2();   // "ClassC::vfunc2()"    C类的虚函数表重写vfunc2

    ClassB* b = new ClassC;
    b->func1();   // "ClassB::func1()"	    普通调用 此时B类的func1 覆盖了A类的方法
    b->func2();   // "ClassA::func2()"	    B类 func2直接继承的A类的方法
    b->vfunc1();   // "ClassB::vfunc1()"	C类的虚函数表继承B类虚函数表 B类虚函数重写vfunc1
    b->vfunc2();   // "ClassC::vfunc2()"	C类的虚函数表重写把vfunc2()
}


void test_this()
{
    auto            c = std::make_shared<Container>();
    DerivedTestThis d(c);
    auto            ptr = c->get_ptr();
    ptr->func();
}

int main()
{
    // test_class_memory();

    //    test_virtual_class();

    //    test_multi_public();

    //    test_class_constructor();

    //    test_class_special_mem_func();

    test_this();
    return 0;
}