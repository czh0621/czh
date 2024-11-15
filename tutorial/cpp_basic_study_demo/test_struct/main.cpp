//
// Created by viruser on 2024/11/15.
//

#include "test_struct.h"

void test_struct_memory()
{
    spdlog::info("TestStruct {}", sizeof(TestStruct));
    spdlog::info("TestStruct2 {}", sizeof(TestStruct2));
    spdlog::info("TestStruct3 {}", sizeof(TestStruct3));
}

void test_struct_to_byte_array()
{
    TestStruct s1;
    s1.num = 1;
    strcpy(s1.message, "a");
    s1.money = 2;
    char c[50];
    memcpy(&c, &s1, sizeof(s1));   // 写入流的方式按照TestStruct内存对齐写入的
    TestStruct s2;
    memcpy(&s2, c, sizeof(s2));
    spdlog::info("byte to TestStruct2 use equal pack {} {} {}", s2.num, s2.message, s2.money);

    TestStruct3 s3;   // sizeof(s3) == sizeof(s1) 所以不会错 内存对齐了
    memcpy(&s3, c, sizeof(s3));
    spdlog::error("byte to TestStruct3 use non-equal pack {} {} {}", s3.num, s3.message, s3.money);

    // error 内存对齐规则 不一样
    // 此处使用的TestStruct3的内存对齐方式去解包，而写入的方式为内存对齐方式
    TestStruct3 s31;
    memcpy(&s31.num, c, sizeof(s31.num));
    memcpy(&s31.message, c + sizeof(s31.num), sizeof(s31.message));
    memcpy(&s31.money, c + sizeof(s31.num) + sizeof(s31.message), sizeof(s31.money));
    spdlog::error(
        "byte to TestStruct3-1 use non-equal pack {} {} {}", s31.num, s31.message, s31.money);

    char        c2[50];
    std::string s;
    s.append((const char*)&s1.num, sizeof(s1.num));
    s.append((const char*)&s1.message, sizeof(s1.message));
    s.append((const char*)&s1.money, sizeof(s1.money));
    // 按照一字节写入流
    memcpy(&c2, s.c_str(), s.length());

    //
    TestStruct3 s32;
    // memcopy pod 类型
    memcpy(&s32.num, c2, sizeof(s32.num));
    memcpy(&s32.message, c2 + sizeof(s32.num), sizeof(s32.message));
    memcpy(&s32.money, c2 + sizeof(s32.num) + sizeof(s32.message), sizeof(s32.money));

    spdlog::info(
        "byte to TestStruct3-2 use non-equal pack {} {} {}", s32.num, s32.message, s32.money);


    TestStruct3 s33;
    int         value;
    char        tmp[50];
    double      d_value;

    //&tmp 获取的是
    spdlog::error("get char addr {} {}", (void*)(tmp + 1), (void*)(&tmp + 1));

    memcpy(&value, c2, sizeof(s33.num));
    memcpy(&tmp, (c2 + sizeof(s33.num)), sizeof(s33.message));
    memcpy(&d_value, (c2 + sizeof(s33.num) + sizeof(s33.message)), sizeof(s33.money));
    s33.num = value;
    strcpy(s33.message, tmp);
    s33.money = d_value;

    spdlog::info(
        "byte to TestStruct3-3 use non-equal pack {} {} {}", s33.num, s33.message, s33.money);
}

int main()
{
    test_struct_memory();
    test_struct_to_byte_array();
    return 0;
}
