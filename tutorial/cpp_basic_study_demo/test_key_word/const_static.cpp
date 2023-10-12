/**
 * @file       const_static.cpp
 * @date       2023/9/12
 * @since      2023-09-12
 * @author     czh
 */

// #include "const_static.h" //全局变量重定义了
#include <spdlog/spdlog.h>
static void g_print()
{
    spdlog::info("g_print call");
}
