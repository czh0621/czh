//
// Created by viruser on 2024/11/15.
//

#ifndef CZH_TEST_STRUCT_H_
#define CZH_TEST_STRUCT_H_

#include <spdlog/spdlog.h>


// 默认8
struct TestStruct
{
    TestStruct() { memset(this, 0, sizeof(TestStruct)); }
    int    num;
    char   message[30];
    double money;
};


struct alignas(8) TestStruct2
{
    TestStruct2() { memset(this, 0, sizeof(TestStruct2)); }
    int    num;
    char   message[30];
    double money;
};

#pragma pack(push, 4)
struct TestStruct3
{
    TestStruct3() { memset(this, 0, sizeof(TestStruct3)); }
    int    num;
    char   message[30];
    double money;
};
#pragma pack(pop)

#endif   // CZH_TEST_STRUCT_H_
