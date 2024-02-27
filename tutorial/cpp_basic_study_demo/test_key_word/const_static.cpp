/**
 * @file       const_static.cpp
 * @date       2023/9/12
 * @since      2023-09-12
 * @author     czh
 */

#include "const_static.h"   //全局变量重定义了
#include <string>

// cpp 中进行类外初始化
std::string const_static::m_s = "static_member";

#include <spdlog/spdlog.h>
static void g_print()
{
    spdlog::info("g_print call");
}
