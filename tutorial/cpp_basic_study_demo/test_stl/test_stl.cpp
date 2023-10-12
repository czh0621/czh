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


int main()
{
    //    test_object_operator();
    //    test_object_cmp();
    //    test_object_func_cmp();

    test_unordered_set();

    test_priority_queue();
}