/**
 * @file       test_stl.cpp
 * @date       2023/10/11
 * @since      2023-10-11
 * @author     czh
 */
#include <algorithm>
#include <iostream>
#include <map>
#include <queue>
#include <spdlog/spdlog.h>
#include <thread>
#include <type_traits>
#include <unordered_set>
#include <vector>

class CustomObject
{
public:
    int value;

    CustomObject(int val)
        : value(val)
    {}

    bool operator<(const CustomObject& other) const { return value < other.value; }
};

class CustomObject2
{
public:
    int value;

    CustomObject2(int val)
        : value(val)
    {}
};

struct CustomObject2Cmp
{
    inline bool operator()(const CustomObject2& first, const CustomObject2& second) const
    {
        return first.value > second.value;
    }
};

inline bool g_cmp(const CustomObject2& first, const CustomObject2& second)
{
    return first.value < second.value;
}

void test_object_operator()
{
    std::vector<CustomObject> vec = {CustomObject(3), CustomObject(1), CustomObject(2)};

    // 对vector进行排序 需重写operator<
    std::sort(vec.begin(), vec.end());
    std::cout << "sort: ";
    for (auto& item : vec) {
        std::cout << item.value << " ";
    }
    std::cout << std::endl;
}

void test_object_cmp()
{
    std::vector<CustomObject2> vec = {CustomObject2(3), CustomObject2(1), CustomObject2(2)};

    // 对vector进行排序 需重写自定义对象operator()
    std::sort(vec.begin(), vec.end(), CustomObject2Cmp());
    std::cout << "sort: ";
    for (auto& item : vec) {
        std::cout << item.value << " ";
    }
    std::cout << std::endl;
}

void test_object_func_cmp()
{
    std::vector<CustomObject2> vec  = {CustomObject2(3), CustomObject2(1), CustomObject2(2)};
    std::vector<CustomObject2> vec2 = {CustomObject2(3), CustomObject2(1), CustomObject2(2)};
    // 对vector进行排序 直接传入比较函数
    std::sort(vec.begin(), vec.end(), g_cmp);
    std::cout << "sort: ";
    for (auto& item : vec) {
        std::cout << item.value << " ";
    }
    std::cout << std::endl;

    std::sort(
        vec2.begin(), vec2.end(), [](const CustomObject2& first, const CustomObject2& second) {
            return g_cmp(first, second);
        });
    std::cout << "lambda sort: ";
    for (auto& item : vec2) {
        std::cout << item.value << " ";
    }
    std::cout << std::endl;
}

class SCustomObject
{
public:
    int value;

    SCustomObject(int val)
        : value(val)
    {}
    // 内部
    bool operator==(const SCustomObject& other) const { return value == other.value; }
};

struct CustomObjectHash
{
    inline std::size_t operator()(const SCustomObject& obj) const
    {
        return std::hash<int>()(obj.value);
    }
};

struct CustomObjectCmp
{
    inline bool operator()(const SCustomObject& obj1, const SCustomObject& obj2) const
    {
        return obj1.value == obj2.value;
    }
};


void test_unordered_set()
{
    std::unordered_set<SCustomObject, CustomObjectHash, CustomObjectCmp> set(
        10, CustomObjectHash(), CustomObjectCmp());
    set.emplace(SCustomObject(3));
    set.emplace(SCustomObject(2));
    set.emplace(SCustomObject(5));
    std::cout << "unordered_set: ";
    for (auto& item : set) {
        std::cout << item.value << " ";
    }
    std::cout << std::endl;

    // 使用Lambda表达式作为哈希函数
    auto customHash = [](const SCustomObject& obj) { return std::hash<int>()(obj.value); };

    // 使用Lambda表达式作为比较函数
    auto customCmp = [](const SCustomObject& obj1, const SCustomObject& obj2) {
        return obj1.value == obj2.value;
    };

    // 使用自定义的哈希函数和比较函数
    std::unordered_set<SCustomObject, decltype(customHash), decltype(customCmp)> customSet(
        10, customHash, customCmp);
}

void test_priority_queue()
{
    // 使用默认cmp 通过类重写 operator<即可
    std::priority_queue<CustomObject> prior_q;
    prior_q.emplace(CustomObject(3));
    prior_q.emplace(CustomObject(1));
    prior_q.emplace(CustomObject(5));
    std::cout << "prior_q: ";
    while (!prior_q.empty()) {
        std::cout << prior_q.top().value << " ";
        prior_q.pop();
    }
    std::cout << std::endl;

    // 显示指定模板
    std::priority_queue<CustomObject2,
                        std::vector<CustomObject2>,
                        std::function<bool(const CustomObject2& obj1, const CustomObject2& obj2)>>
        prior_queue([](const CustomObject2& obj1, const CustomObject2& obj2) {
            return obj1.value < obj2.value;
        });
    prior_queue.emplace(CustomObject2(3));
    prior_queue.emplace(CustomObject2(1));
    prior_queue.emplace(CustomObject2(5));
    std::cout << "prior_q: ";
    while (!prior_queue.empty()) {
        std::cout << prior_queue.top().value << " ";
        prior_queue.pop();
    }
    std::cout << std::endl;


    // 不能使用decltype(g_cmp) 原因  _Compare   comp; 不能使用函数原型定义对象
    // 使用decay退化函数指针，否则必须使用decltype(&g_cmp)
    std::
        priority_queue<CustomObject2, std::vector<CustomObject2>, std::decay<decltype(g_cmp)>::type>
            prior_queue2(g_cmp);   // 此处的g_cmp 会退化成函数指针类型
    prior_queue2.emplace(CustomObject2(3));
    prior_queue2.emplace(CustomObject2(1));
    prior_queue2.emplace(CustomObject2(5));
    std::cout << "decltype prior_q: ";
    while (!prior_queue.empty()) {
        std::cout << prior_queue.top().value << " ";
        prior_queue.pop();
    }
    std::cout << std::endl;
}

class NoCopyObject
{
public:
    NoCopyObject()                        = default;
    NoCopyObject(const NoCopyObject& obj) = delete;
    NoCopyObject(NoCopyObject&& obj) noexcept { spdlog::info("call move NoCopyObject"); }
};

class NoMoveObject
{
public:
    NoMoveObject() = default;
    NoMoveObject(const NoMoveObject& obj) { spdlog::info("call copy NoMoveObject"); }
    //    NoMoveObject(NoMoveObject&& obj) = delete;
};

class Object
{
public:
    Object() = default;
    Object(const Object& obj) { spdlog::info("call copy Object"); }
    Object(Object&& obj) noexcept { spdlog::info("call move Object"); }
};



void test_vector()
{
    std::vector<NoCopyObject> v1;
    std::vector<Object>       v2;
    std::vector<NoMoveObject> v3;
    NoCopyObject              c_obj1;
    Object                    obj1;
    NoMoveObject              m_obj1;


    //    spdlog::info("push_back -------- left value ");
    //    // left value
    //    //    v1.push_back(c_obj1);   // should call copy but delete copy
    //    v2.push_back(obj1);
    //    v3.push_back(m_obj1);
    //
    //    spdlog::info("emplace_back -------- left value ");
    //    //     v1.emplace_back(c_obj1);   // should call copy but delete copy
    //    v2.emplace_back(obj1);   // 1th copy ->  2th move ----- if Object(Object&& obj) is not
    //    noexcept
    //                             // call copy else call move
    //    v3.emplace_back(m_obj1);   // 1th copy ->  2th must be copy -----because
    //                               // NoMoveObject(NoMoveObject&& obj) = delete;


    // right value
    spdlog::info("push_back -------- right value");
    //    NoCopyObject&& n_tmp = NoCopyObject();
    //    auto           value = std::is_same<decltype(n_tmp), NoCopyObject&&>::value;
    //    v1.push_back(std::move(n_tmp));   // 直接调用push_back右值引用函数 only call move
    //    spdlog::info("n_tmp type is NoCopyObject&&:{}", value);
    v1.push_back(NoCopyObject());   // 直接调用push_back右值引用函数 only call move
                                    //    Object&& tmp = Object();
    //    value        = std::is_same<decltype(tmp), Object&&>::value;
    //    spdlog::info("tmp type is Object&&:{}", value);
    v2.push_back(Object());
    //    NoMoveObject&& m_tmp = NoMoveObject();
    //    value                = std::is_same<decltype(m_tmp), NoMoveObject&&>::value;
    //    spdlog::info("m_tmp type is NoMoveObject&&:{}", value);
    //    v3.push_back(std::move(m_tmp));
    v3.push_back(NoMoveObject());

    //    spdlog::info("emplace_back -------- right value");
    //    v1.emplace_back(NoCopyObject());
    //    v2.emplace_back(std::move(obj1));
    //    v3.emplace_back(std::move(m_obj1));
}


int main()
{
    //    test_object_operator();
    //    test_object_cmp();
    //    test_object_func_cmp();

    //    test_unordered_set();
    //
    //    test_priority_queue();

    test_vector();
}