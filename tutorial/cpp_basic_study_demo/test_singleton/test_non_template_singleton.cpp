//
// Created by chenzhihao on 2024/11/10.
//

#include "test_non_template_singleton.h"
test_non_template_singleton& test_non_template_singleton::instance()
{
    static test_non_template_singleton p_singleton;
    return p_singleton;
}
